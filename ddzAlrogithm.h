#ifndef __DDZ_ALROGITHM_H__
#define __DDZ_ALROGITHM_H__

#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <tuple>


namespace ddzalgorithm{
enum class CardNum{
    UNKNOWN=     -0x1,
    
    THREE =     0x0,
    FOUR  =     0x1,
    FIVE  =     0x2,
    SIX   =     0x3,
    SEVEN =     0x4,

    EIGHT =     0x5,
    NINE  =     0x6,
    TEN   =     0x7,
    JACK  =     0x8,
    QUEEN =     0x9,

    KING  =     0xa,
    ACE   =     0xb,
    TWO   =     0xc,
};

enum class CardType{
    HEART           = 0x0,
    SPADE           = 0x1,
    CLUB            = 0x2,
    DIAMONAD        = 0x3,

    LAIZI           = 0x4,
    SMALL_JOKER     = 0x5,
    BIG_JOKER       = 0x6,
};

struct Card{
    Card(uint8_t input){
        origin = input;
        type = static_cast<CardType>((input & 0xf0 >> 8));
        num = static_cast<CardNum>(input & 0x0f);
    }
    uint8_t origin;
    CardType type;
    CardNum num;

    //获取相似判断编码 ，不能超过0xf
    int getRecordCode() const {
        //普通牌和癞子不受type影响
        if(static_cast<int>(type) <= static_cast<int>(CardType::LAIZI)){
            // 癞子牌被当作非癞子牌使用
            return static_cast<int>(num);
        }else if(type == CardType::SMALL_JOKER){
            return 0xe;
        }else{
            return 0xf;
        }
    }
};

enum class Illegal{
    ROCKET                      = 0xff,        

    // not change bomb order
    BOMB_8_PURE_JOKER           = 0xef,        
    BOMB_8_COMB                 = 0xee,        
    BOMB_7_PURE_JOKER           = 0xed,        
    BOMB_7_COMB                 = 0xec,        
    BOMB_6_PURE_JOKER           = 0xeb,        
    BOMB_6_COMB                 = 0xea,
    BOMB_5_PURE_JOKER           = 0xe9,
    BOMB_5_COMB                 = 0xe8,
    BOMB_4_PURE_JOKER           = 0xe7,        
    BOMB_4_COMB                 = 0xe6,        
    BOMB_4_WITHOU_JOKER         = 0xe5,        

    
    SINGLE                      = 0x10,

    PAIR                        = 0x20,

    TRIPLE_PURE                 = 0x30,
    TRIPLE_WITH_SINGLE          = 0x31,
    TRIPLE_WITH_PAIR            = 0x31,

    //not change chain order and value
    CHAIN_5                     = 0x40,
    CHAIN_6                     = 0x41,
    CHAIN_7                     = 0x42,
    CHAIN_8                     = 0x43,
    CHAIN_9                     = 0x44,
    CHAIN_10                    = 0x45,
    CHAIN_11                    = 0x46,
    CHAIN_12                    = 0x47,          //3 ~ ACE 12cards

    //ont change pair chains order and value
    PAIR_CHAIN_3                = 0x50,
    PAIR_CHAIN_4                = 0x51,
    PAIR_CHAIN_5                = 0x52,
    PAIR_CHAIN_6                = 0x53,
    PAIR_CHAIN_7                = 0x54,
    PAIR_CHAIN_8                = 0x55,     
    PAIR_CHAIN_9                = 0x56,     
    PAIR_CHAIN_10               = 0x57,     // 20 cards

    FOUR_WITH_TWO_SINGLES        = 0x60,    //6
    FOUR_WITH_TWO_PAIRS          = 0x60,    //8

    //飞机的三条部分不能带2
    AIRPLANE_2_NOT_SWINGS        = 0x70,    //6
    AIRPLANE_3_NOT_SWINGS        = 0x71,
    AIRPLANE_4_NOT_SWINGS        = 0x72,
    AIRPLANE_5_NOT_SWINGS        = 0x73,     //15 cards
    AIRPLANE_6_NOT_SWINGS        = 0x74,     //18 cards

    AIRPLANE_2_WITH_SMALL_SWINGS     = 0x75, //8 cards
    AIRPLANE_3_WITH_SMALL_SWINGS     = 0x76, //12 cards
    AIRPLANE_4_WITH_SMALL_SWINGS     = 0x77, //16 cards
    AIRPLANE_5_WITH_SMALL_SWINGS     = 0x78, //20 cards

    AIRPLANE_2_WITH_BIG_SWINGS       = 0x79, // 10 cards
    AIRPLANE_3_WITH_BIG_SWINGS       = 0x7a, // 15 cards
    AIRPLANE_4_WITH_BIG_SWINGS       = 0x7b, // 20 cards

    //航天飞机
    SPACE_SHUTTLE_2_NOT_SWINGS                   = 0x80,//3333 4444
    SPACE_SHUTTLE_3_NOT_SWINGS                   = 0x81,//
    SPACE_SHUTTLE_4_NOT_SWINGS                   = 0x82,//16 cards

    SPACE_SHUTTLE_2_WITH_SWINGS                   = 0x83,//3333 4444 5678
    SPACE_SHUTTLE_3_WITH_SWINGS                   = 0x84,//16 cards
};

struct Token{
    Token(Illegal p1,std::vector<uint8_t> p2):
        type(p1),
        data(p2){}
    Illegal type;
    std::vector<uint8_t> data;
};

/**
 * @return
 *      illegal token vector     
 * 
 *      "*" represent laizi 
 * @input  
 *      input cards
 * @reduceSameBomb  
 *      eg: 22 8(*)888 can be combinated 2288 or 2888,
 *      if true, only has one item that is 2288(use min laizi)
 *      elsewise,2288 or 2888 all can be added to result
 * @reduceSamePair
 *      eg: 2 8(*) 9(*) can be combinated 28 or 29
 *      if true, only has one item
 *      elsewise,28 or 29 all can be added to result
 * @reduceSameThreeWithOne
 *      eg: 22 8(*)88 9(*)99 can be combinated 2288 or 2289
 *      if true, oney has one item
 *      elsewise,2288 2289 all can be added to result
 * @reduceSameThreeWithPair
 *      eg: 22 8(*)88 9(*)99 can be combinated 228 88 or 228 99
 *      if true,only has one item
 *      elsewise, 228 88 or 228 99 all can be added to result
 * @maxAirPlane
 *      max airplane three part value(defualt enable)
 *      任何情況下，其三條部分都不能有2 from wikipedia ,i dont know
 * @supportShuttle
 *      support space shuttle
 *      
 */
std::vector<Token> getToken(std::vector<uint8_t> input,
    bool reduceSameBomb = false,
    bool reduceSamePair = false,
    bool reduceSameThreeWithOne = false,
    bool reduceSameThreeWithPair = false,
    int maxAirplane = 12,
    bool supportShuttle = false
    )
{
    std::vector<Token> result;
    auto totalCardNum = input.size();
    std::vector<Card> parseCard(totalCardNum);
    for(const auto& one : input)
    {
        parseCard.emplace_back(one);
    }

    // get info            3 4 5 6 7,  8 9 10j q   k A 2
    // int cardNums[13] = {0,0,0,0,0,  0,0,0,0,0,  0,0,0};
    const Card* smallJoker = nullptr;
    const Card* bigJoker = nullptr;
    const Card* laiziCards[2 * 4] = {nullptr}; 
    const Card* normalCards[13][4] = {nullptr};
    int normalCardsNum[13] = {0};
    int laiziCardsNum = 0;
    for(const auto& one: parseCard)
    {
        if(one.type == CardType::LAIZI){
            laiziCards[laiziCardsNum++] = &one;
        }else if(one.type == CardType::SMALL_JOKER){
            smallJoker = &one;
        }else if(one.type == CardType::BIG_JOKER){
            bigJoker = &one;
        }else{
            int num = static_cast<int>(one.num);
            normalCards[num][normalCardsNum[num]++] = &one;
        }
    }
    int maxNormalCardsNum = 0;
    for(int i = 0; i < 13; i++){
        auto num = normalCardsNum[i];
        if(num > maxNormalCardsNum){
            maxNormalCardsNum = num;
        }
    }
    // 天地癞子
    const Card* firstLaiziCards[4];
    const Card* secondLaiziCards[4];
    int firstLaiziCardsNum = 0;
    int secondLaiziCardsNum = 0;
    if(laiziCardsNum > 0){
        const CardNum cardNum = laiziCards[0]->num;
        firstLaiziCards[firstLaiziCardsNum++] = laiziCards[0];
        for(int i = 1; i < laiziCardsNum; i++){
            if(cardNum == laiziCards[i]->num){
                firstLaiziCards[firstLaiziCardsNum++] = laiziCards[i];
            }else{
                secondLaiziCards[secondLaiziCardsNum++] = laiziCards[i];
            }
        }
    }

    //rocket
    if(smallJoker != nullptr && bigJoker != nullptr){
        std::vector<uint8_t> rocket{smallJoker->origin,bigJoker->origin};
        result.emplace_back(Illegal::ROCKET,rocket);
    }

    //bomb max 13 * 5 * 4 == 260 loop
    if(maxNormalCardsNum + laiziCardsNum >= 4){
        int illegal = static_cast<int>(Illegal::BOMB_4_WITHOU_JOKER);
        for(int i = 4; i <= 8; i++){
            //bomb pure joker，no need to select use those laizi card combination
            bool findBomb = false;
            if(i <= laiziCardsNum){
                std::vector<uint8_t> bomb;
                for(int j = 0; j < i; j++){
                    bomb.emplace_back(laiziCards[j]->origin);
                }
                result.emplace_back(static_cast<Illegal>(illegal + 2),bomb);
                findBomb = true;
            }

            //bomb combination (soft bomb or not laizi bomb)
            auto calcBomb = [&](int useNormalNum,const const Card** normal){
                std::vector<uint8_t> bomb;
                int j = 0;
                for (j = 0; j < useNormalNum; j++){
                    bomb.emplace_back(normal[j]->origin);
                }
                for(int k = 0; k < i - useNormalNum; k++){
                    bomb.emplace_back(laiziCards[k]->origin);
                }
                //特殊情况，当没有使用癞子，则为正常炸弹
                if(useNormalNum == 4 && i == 4){
                    result.emplace_back(static_cast<Illegal>(illegal),bomb);
                }else{
                    result.emplace_back(static_cast<Illegal>(illegal + 1),bomb);
                }
            };
            if(i <= maxNormalCardsNum + laiziCardsNum){
                for(int j = 0; j < 13; j++){
                    auto normalNum = normalCardsNum[j];
                    if(normalNum > 0 && normalNum + laiziCardsNum >= i){
                        findBomb = true;
                        auto normal = normalCards[j];
                        if(reduceSameBomb){
                            //4软炸这里不会生成，只会生成无癞子炸弹，需要手动补上，因为4软炸比4无癞子炸弹大
                            if(normalNum == 4 && i == 4 && laiziCardsNum >= 1){
                                calcBomb(3,normal);
                            }
                            calcBomb(normalNum,normal);
                        }else{
                            // 22 8(*)888 can be combinated with 2288 or 2888 
                            for(int useNormalNum = 1; useNormalNum <= normalNum; useNormalNum++){
                                if(useNormalNum + laiziCardsNum >= i){
                                    calcBomb(useNormalNum,normal);
                                }
                            }
                        }
                    }
                }
            }
            illegal = illegal + 2;
            if(!findBomb){
                break;
            }
        }
    }

    //single 
    if (totalCardNum >= 1){
        std::vector<uint8_t> tempSingles;
        for(int i = 0; i < 13; i++){
            if(normalCardsNum[i] > 0){
                tempSingles.emplace_back(normalCards[i][0]->origin);
            }
        }
        if(laiziCardsNum > 0){
            if(firstLaiziCardsNum > 0){
                tempSingles.emplace_back(firstLaiziCards[0]->origin);
            }
            if(secondLaiziCardsNum > 0){
                tempSingles.emplace_back(secondLaiziCards[0]->origin);
            }
        }
        if(smallJoker != nullptr){
            tempSingles.emplace_back(smallJoker->origin);
        }
        if(bigJoker != nullptr){
            tempSingles.emplace_back(bigJoker->origin);
        }
        for(const auto& one : tempSingles){
            std::vector<uint8_t> single{one};
            result.emplace_back(Illegal::SINGLE,single);
        }
    }

    //pair
    if(maxNormalCardsNum + laiziCardsNum >= 2){
        for(int i = 0; i < 13; i++){
            int num = normalCardsNum[i];
            if(num > 0 && num + laiziCardsNum >= 2){
                const auto& one = normalCards[i];
                if(num == 1){
                    if(reduceSamePair){
                        std::vector<uint8_t> pair;
                        pair.emplace_back(one[0]->origin);
                        pair.emplace_back(laiziCards[0]->origin);
                        result.emplace_back(Illegal::PAIR,pair);
                    }else{
                        if(firstLaiziCardsNum > 0){
                            std::vector<uint8_t> pair;
                            pair.emplace_back(one[0]->origin);
                            pair.emplace_back(firstLaiziCards[0]->origin);
                            result.emplace_back(Illegal::PAIR,pair);
                        }
                        if(secondLaiziCardsNum > 0){
                            std::vector<uint8_t> pair;
                            pair.emplace_back(one[0]->origin);
                            pair.emplace_back(secondLaiziCards[0]->origin);
                            result.emplace_back(Illegal::PAIR,pair);
                        }
                    }
                }else{
                    std::vector<uint8_t> pair;
                    pair.emplace_back(one[0]->origin);
                    pair.emplace_back(one[1]->origin);
                    result.emplace_back(Illegal::PAIR,pair);
                }
            }
        }
        if(firstLaiziCardsNum >= 2){
            std::vector<uint8_t> pair{firstLaiziCards[0]->origin,firstLaiziCards[1]->origin};
            result.emplace_back(Illegal::PAIR,pair);
        }
        if(secondLaiziCardsNum >= 2){
            std::vector<uint8_t> pair{secondLaiziCards[0]->origin,secondLaiziCards[1]->origin};
            result.emplace_back(Illegal::PAIR,pair);
        }
        //rocket is not pair
    }

    //triple 
    if(maxNormalCardsNum + laiziCardsNum >= 3){
        //1. normal as triple(exclude laizi as triple)
        for(int i = 0; i < 13; i++){
            int num = normalCardsNum[i];
            if(num > 0 && num + laiziCardsNum >= 3){
                const auto& one = normalCards[i];
                std::vector<uint8_t> triple;
                int useLaiziNum = 0;
                if(num == 1){
                    triple.emplace_back(one[0]->origin);
                    triple.emplace_back(laiziCards[0]->origin);
                    triple.emplace_back(laiziCards[1]->origin);
                    useLaiziNum = 2;
                }else if(num == 2){
                    triple.emplace_back(one[0]->origin);
                    triple.emplace_back(one[1]->origin);
                    triple.emplace_back(laiziCards[0]->origin);
                    useLaiziNum = 1;
                }else{
                    triple.emplace_back(one[0]->origin);
                    triple.emplace_back(one[1]->origin);
                    triple.emplace_back(one[2]->origin);
                }
                result.emplace_back(Illegal::TRIPLE_PURE,triple);

                //triple with one
                if(totalCardNum >= 4){
                    std::vector<uint8_t> tempSingles;
                    for(int j = 0; j < 13; j++){
                        if(normalCardsNum[j] > 0 && j != i){
                            tempSingles.emplace_back(normalCards[j][0]->origin);
                        }
                    }
                    if(smallJoker != nullptr){
                        tempSingles.emplace_back(smallJoker->origin);
                    }
                    if(bigJoker != nullptr){
                        tempSingles.emplace_back(bigJoker->origin);
                    }
                    auto calcTripleSingle = [&](){
                        for(const auto& one: tempSingles){
                            std::vector<uint8_t> copyTriple(triple);
                            copyTriple.emplace_back(one);
                            result.emplace_back(Illegal::TRIPLE_WITH_SINGLE,copyTriple);
                        }
                    };
                    // spec: laizi combination
                    if(useLaiziNum == 0){
                        //no use laizi
                        if(reduceSameThreeWithOne){
                            if(laiziCardsNum > 0){
                                tempSingles.emplace_back(laiziCards[0]->origin);
                            }
                        }else{
                            if(firstLaiziCardsNum > 0){
                                tempSingles.emplace_back(firstLaiziCards[0]->origin);
                            }
                            if(secondLaiziCardsNum > 0){
                                tempSingles.emplace_back(secondLaiziCards[0]->origin);
                            }
                        }
                        calcTripleSingle();
                    }else if(useLaiziNum == laiziCardsNum){
                        //all laizi be used
                        calcTripleSingle();
                    }else{
                        if(laiziCardsNum - useLaiziNum >= 1){
                            // has no use laizi cards
                            if(!reduceSameThreeWithOne && firstLaiziCardsNum > 0 && secondLaiziCardsNum > 0){
                                // two types laizi, can be 
                                auto calcTripleSingle2 = [&](int firstNum,const Card** first,int secondNum,const Card** second){
                                    std::vector<uint8_t> copyTriple(triple);
                                    std::vector<uint8_t> tempSingles2(tempSingles);
                                    std::vector<uint8_t> priorityLaiZi;
                                    for(int i = 0; i < firstNum; i++){
                                        priorityLaiZi.emplace_back(first[i]->origin);
                                    }
                                    for(int i = 0; i < secondNum; i++){
                                        priorityLaiZi.emplace_back(second[i]->origin);
                                    }
                                    int index = 0;
                                    for(int i = 3 - useLaiziNum; i < 3; i++){
                                        copyTriple[i] = priorityLaiZi[index++];
                                    }
                                    tempSingles2.emplace_back(second[secondNum - 1]->origin);
                                    for(const auto& one: tempSingles2){
                                        std::vector<uint8_t> copyTriple2(copyTriple);
                                        copyTriple2.emplace_back(one);
                                        result.emplace_back(Illegal::TRIPLE_WITH_SINGLE,copyTriple2);
                                    }
                                };
                                calcTripleSingle2(firstLaiziCardsNum,firstLaiziCards,secondLaiziCardsNum,secondLaiziCards);
                                calcTripleSingle2(secondLaiziCardsNum,secondLaiziCards,firstLaiziCardsNum,firstLaiziCards);
                            }else{
                                // not need to resort use laizi
                                tempSingles.emplace_back(laiziCards[useLaiziNum]->origin);
                                calcTripleSingle();
                            }
                        }
                    }
                }

                //triple with pair
                int leftLaizi = laiziCardsNum - useLaiziNum;
                if(maxNormalCardsNum + leftLaizi >= 2 && totalCardNum >= 5){
                    std::vector<std::tuple<uint8_t,uint8_t>> tempPairs;
                    for(int j = 0; j < 13; j++){
                        int num2 = normalCardsNum[j];
                        if(num2 > 0 && i != j && num2 + leftLaizi >= 2){
                            const auto& one = normalCards[j];
                            if(num2 == 1){
                                tempPairs.emplace_back(one[0]->origin,laiziCards[useLaiziNum]->origin);
                            }else{
                                tempPairs.emplace_back(one[0]->origin,one[1]->origin);
                            }
                        }
                    }
                    for(const auto& one : tempPairs){
                        std::vector<uint8_t> copyTriple(triple);
                        copyTriple.emplace_back(std::get<0>(one));
                        copyTriple.emplace_back(std::get<1>(one));
                        result.emplace_back(Illegal::TRIPLE_WITH_PAIR,copyTriple);
                    }
                    //need resort use laizi,because laizi can be pair eg(33 444(*) 55 can be 334 44 or 334 55)
                    if(leftLaizi >= 2){
                        auto calcTriplePair = [&](int firstNum,const Card** first,int secondNum,const Card** second){
                            std::vector<uint8_t> copyTriple(triple);
                            std::vector<const Card*> priorityLaiZi;
                            for(int i = 0; i < firstNum; i++){
                                priorityLaiZi.emplace_back(first[i]);
                            }
                            for(int i = 0; i < secondNum; i++){
                                priorityLaiZi.emplace_back(second[i]);
                            }
                            int index = 0;
                            for(int i = 3 - useLaiziNum;i < 3; i--){
                                const auto& card = priorityLaiZi[index++];
                                copyTriple[i] = card->origin;
                            }
                            CardNum desireCardNum = second[0]->num;
                            const auto& lastTwoOfCard = priorityLaiZi[laiziCardsNum - 2];
                            const auto& lastFirstOfCard = priorityLaiZi[laiziCardsNum - 1];
                            //最后两个一定要和期望的匹配
                            if (desireCardNum == lastTwoOfCard->num && desireCardNum == lastFirstOfCard->num){
                                copyTriple[4] = lastTwoOfCard->origin;
                                copyTriple[5] = lastFirstOfCard->origin;
                                result.emplace_back(Illegal::TRIPLE_WITH_PAIR,copyTriple);
                            } 
                        };
                        if(reduceSameThreeWithPair){
                            if(firstLaiziCardsNum >= 2){
                                calcTriplePair(secondLaiziCardsNum,secondLaiziCards,firstLaiziCardsNum,firstLaiziCards);
                            }else if(secondLaiziCardsNum >= 2){
                                calcTriplePair(firstLaiziCardsNum,firstLaiziCards,secondLaiziCardsNum,secondLaiziCards);
                            }
                        }else{
                            if(firstLaiziCardsNum >= 2){
                                calcTriplePair(secondLaiziCardsNum,secondLaiziCards,firstLaiziCardsNum,firstLaiziCards);
                            }
                            if(secondLaiziCardsNum >= 2){
                                calcTriplePair(firstLaiziCardsNum,firstLaiziCards,secondLaiziCardsNum,secondLaiziCards);
                            }
                        }
                    }
                }                
            }
        }

        //2. laizi as triple eg 222 8(*)88 888 as triple
        if(laiziCardsNum >= 3){
            auto calcLaiziTriple = [&](int firstNum,const Card** first,int secondNum, const Card** second){
                int canUserLaiziNum = secondNum;
                std::vector<uint8_t> triple{first[0]->origin,first[1]->origin,first[2]->origin};
                result.emplace_back(Illegal::TRIPLE_PURE,triple);
                if(totalCardNum >= 4){
                    std::vector<uint8_t> tempSingles;
                    for(int i = 0; i < 13; i++){
                        if(normalCardsNum[i] > 0){
                            tempSingles.emplace_back(normalCards[i][0]->origin);
                        }
                    }
                    if(canUserLaiziNum > 0){
                        tempSingles.emplace_back(second[0]->origin);
                    }
                    if(smallJoker != nullptr){
                        tempSingles.emplace_back(smallJoker->origin);
                    }
                    if(bigJoker != nullptr){
                        tempSingles.emplace_back(bigJoker->origin);
                    }
                    for(const auto& one : tempSingles){
                        std::vector<uint8_t> copyTriple(triple);
                        copyTriple.emplace_back(one);
                        result.emplace_back(Illegal::TRIPLE_WITH_SINGLE,copyTriple);
                    }
                }
                int leftLaiziNum = laiziCardsNum - 3;
                const Card* priorityLaiZi[5] = {nullptr};
                int priorityLaiZiNum = 0;
                for(int i = 3; i < firstNum; i++){
                    priorityLaiZi[priorityLaiZiNum++] = first[i]; 
                }
                for(int i = 0; i < secondNum; i++){
                    priorityLaiZi[priorityLaiZiNum++] = second[i]; 
                }
                if(totalCardNum >= 5){
                    std::vector<std::tuple<uint8_t,uint8_t>> tempPairs;
                    for(int i = 0; i <13; i++){
                        int num = normalCardsNum[i]; 
                        if(num > 0 && num + leftLaiziNum >= 2){
                            const auto& normal = normalCards[i];
                            if(num == 1){
                                //only use first laizi, not need to resort left laizi combination
                                tempPairs.emplace_back(normal[0]->origin,priorityLaiZi[0]->origin);
                            }else{
                                tempPairs.emplace_back(normal[0]->origin,normal[1]->origin);
                            }
                        }
                    }
                    //2(*)222 8(*)88 can be combinated 222 88
                    if(secondNum >= 2){
                        tempPairs.emplace_back(second[0]->origin,second[1]->origin);
                    }
                    for(const auto& one : tempPairs){
                        std::vector<uint8_t> copyTriple(triple);
                        copyTriple.emplace_back(std::get<0>(one));
                        copyTriple.emplace_back(std::get<1>(one));
                        result.emplace_back(Illegal::TRIPLE_WITH_PAIR,copyTriple);
                    }
                }
            };
            if(firstLaiziCardsNum >= 3){
                calcLaiziTriple(firstLaiziCardsNum,firstLaiziCards,secondLaiziCardsNum,secondLaiziCards);
            }
            if(secondLaiziCardsNum >= 3){
                calcLaiziTriple(secondLaiziCardsNum,secondLaiziCards,firstLaiziCardsNum,firstLaiziCards);
            }
        }
    }

    if(totalCardNum >= 5){
        for(int i = 5; i < 13; i++){ //5 - 12
            if(i > totalCardNum){
                break;
            }
            bool hasChains = false;
            //j表示i链条从哪里开始
            for(int j = 0; j < 13 - i; j++){ //0 ~ 8 0 ~ 1
                std::vector<uint8_t> chains(i);
                std::vector<int> emptyParts;

                // 获取链条的数据 从j开始的i链条
                for(int k = j; k < i + j; k++){
                    //kIndex表示chains中的位置
                    int kIndex = k - j;
                    if(normalCardsNum[k] > 0){
                        chains[kIndex] = normalCards[k][0]->origin;
                    }else{
                        emptyParts.emplace_back(kIndex);
                    }
                }
                int needLaiziNum = emptyParts.size();
                if(needLaiziNum <= laiziCardsNum){
                    for(int k = 0; k < needLaiziNum; k++){
                        chains[emptyParts[k]] = laiziCards[k]->origin;
                    } 
                    result.emplace_back(static_cast<Illegal>(static_cast<int>(Illegal::CHAIN_5) + i - 5),chains);
                    hasChains = true;
                }
            }
            if(!hasChains){
                break;
            }
        }
    }

    if(totalCardNum >= 6){
        //pair chain
        for(int i = 3; i < 11; i++){ //3 - 10
            int needCardNum = 2 * i;
            if(needCardNum > totalCardNum){
                break;
            }
            bool hasChains = false;
            for(int j = 0; j < 13 - i; j++){ //0 ~ 8 0 ~ 3
                std::vector<uint8_t> chains(needCardNum);
                std::vector<int> emptyParts;

                for(int k = j; k < i + j; k++){
                    const auto& one = normalCards[k];
                    int oneNum = normalCardsNum[k];
                    int kIndex = 2 * (k - j);
                    if(oneNum > 1){
                        chains[kIndex] = one[0]->origin;
                        chains[kIndex + 1] = one[1]->origin;
                    }else if(oneNum > 0){
                        chains[kIndex] = one[0]->origin;
                        emptyParts.emplace_back(kIndex + 1);
                    }
                    else{
                        emptyParts.emplace_back(kIndex);
                        emptyParts.emplace_back(kIndex + 1);
                    }
                }
                int needLaiziNum = emptyParts.size();
                if(needLaiziNum <= laiziCardsNum){
                    for(int k = 0; k < needLaiziNum; k++){
                        chains[emptyParts[k]] = laiziCards[k]->origin;
                    } 
                    result.emplace_back(static_cast<Illegal>(static_cast<int>(Illegal::PAIR_CHAIN_3) + i - 3),chains);
                    hasChains = true;
                }
            }
            if(!hasChains){
                break;
            }
        }
    }

    if(totalCardNum >= 6 && maxNormalCardsNum + laiziCardsNum >= 4){
        // four with two singles
        for(int i = 0; i < 13; i++){
            int num = normalCardsNum[i];
            if(num > 0 && num + laiziCardsNum >= 4){
                const auto& one = normalCards[i];
                std::vector<uint8_t> four;
                int useLaiziNum = 0;
                for(int j = 0; j < num; j++){
                    four.emplace_back(one[j]->origin);
                }
                for(int j = num; j < 4; j++){
                    four.emplace_back(laiziCards[useLaiziNum++]->origin);
                }
                //with two singles
                std::vector<const Card*> tempSingles;
                for(int j = 0; j < 13; j++){
                    int num2 = normalCardsNum[j];
                    if(j != i && num2 > 0){
                        const auto& one2 = normalCards[j];
                        //max record two
                        if(num2 > 1){
                            tempSingles.emplace_back(one2[0]);
                            tempSingles.emplace_back(one2[1]);
                        }else{
                            tempSingles.emplace_back(one2[0]);
                        }
                    }
                }
                if(laiziCardsNum - useLaiziNum > 0){
                    if(laiziCardsNum - useLaiziNum == 1){
                        tempSingles.emplace_back(laiziCards[useLaiziNum]);
                    }else{
                        tempSingles.emplace_back(laiziCards[useLaiziNum]);
                        tempSingles.emplace_back(laiziCards[useLaiziNum + 1]);
                    }
                }
                if(smallJoker != nullptr){
                    tempSingles.emplace_back(smallJoker);
                }
                if(bigJoker != nullptr){
                    tempSingles.emplace_back(bigJoker);
                }
                //唯一编码，去除重复
                unsigned char record[16 * 16 + 16] = {'\0'};
                auto p2Size = tempSingles.size();
                if(p2Size >= 2){
                    //移除相似的部分,冒泡算法遍历组合
                    for(int i = 0; i < p2Size; i++){
                        for(int j = i + 1; j < p2Size; j++){
                            const Card* first = tempSingles[i];
                            const Card* second = tempSingles[j];
                            int firstKey = first->getRecordCode();
                            int secondKey = second->getRecordCode();
                            int key1 = firstKey * 16 + secondKey;
                            int key2 = secondKey * 16 + firstKey;
                            if(record[key1] == '\0' && record[key2] == '\0'){
                                record[key1] = '\1';
                                record[key2] = '\1';
                                std::vector<uint8_t> copyFour(four);
                                copyFour.emplace_back(first->origin);
                                copyFour.emplace_back(second->origin);
                                result.emplace_back(Illegal::FOUR_WITH_TWO_SINGLES,copyFour);
                            }
                        }
                    }
                }

                //four with two pairs
            }
        }
    }

    //four with two pairs
    if(totalCardNum >= 8 && maxNormalCardsNum + laiziCardsNum >= 4){
        const Card* priorityLaizi[8];
        int tempLaiziNum = 0;
        for(int i = 0; i < firstLaiziCardsNum; i++){
            priorityLaizi[tempLaiziNum++] = firstLaiziCards[i];
        }
        for(int i = 0; i < secondLaiziCardsNum; i++){
            priorityLaizi[tempLaiziNum++] = secondLaiziCards[i];
        }
        for(int i = 0; i < 13; i++){
            int num = normalCardsNum[i];
            if(num > 0 && num + laiziCardsNum >= 4){
                const auto& one = normalCards[i];
                std::vector<uint8_t> four;
                const Card* first[2];
                const Card* second[2];
                int fourUseLaizi = 0;
                for(int j = 0; j < num; j++){
                    four.emplace_back(one[j]->origin);
                }
                for(int j = num; j < 4; j++){
                    four.emplace_back(priorityLaizi[fourUseLaizi++]->origin);
                }

                //find first pair
                int leftLaiziNum = laiziCardsNum - fourUseLaizi;
                for(int j = 0; j < 13; j++){
                    int firstNum = normalCardsNum[j];
                    if(firstNum > 0 && firstNum + leftLaiziNum >= 2 && j != i){
                        int firstUseLaizi = 0;
                        const auto& firstCards = normalCards[j];
                        first[0] = firstCards[0];
                        if(firstNum == 1){
                            first[1] = priorityLaizi[fourUseLaizi + (firstUseLaizi++)];
                        }else{
                            first[1] = firstCards[1];
                        }
                        for(int k = j + 1; k < 13; k++){
                            int secondNum = normalCardsNum[k];
                            if(secondNum > 0 && secondNum + (leftLaiziNum - firstUseLaizi) >= 2 && k != i){
                                const auto& secondCards = normalCards[k];
                                second[0] = secondCards[0];
                                if(secondNum == 1){
                                    second[1] = priorityLaizi[fourUseLaizi + firstUseLaizi];
                                }else{
                                    second[1] = secondCards[1];
                                }
                                std::vector<uint8_t> copyFour(four);
                                copyFour.emplace_back(first[0]);
                                copyFour.emplace_back(first[1]);
                                copyFour.emplace_back(second[0]);
                                copyFour.emplace_back(second[1]);
                                result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                            }
                        }
                        if(firstLaiziCardsNum - fourUseLaizi - firstUseLaizi >= 2){
                            std::vector<uint8_t> copyFour(four);
                            copyFour.emplace_back(first[0]);
                            copyFour.emplace_back(first[1]);
                            copyFour.emplace_back(firstLaiziCards[firstLaiziCardsNum - 1]);
                            copyFour.emplace_back(firstLaiziCards[firstLaiziCardsNum - 2]);
                            result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                        } 
                        if(laiziCardsNum - fourUseLaizi - firstUseLaizi >= 2 && secondLaiziCardsNum >= 2){
                            std::vector<uint8_t> copyFour(four);
                            copyFour.emplace_back(first[0]);
                            copyFour.emplace_back(first[1]);
                            copyFour.emplace_back(secondLaiziCards[secondLaiziCardsNum - 1]);
                            copyFour.emplace_back(secondLaiziCards[secondLaiziCardsNum - 2]);
                            result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                        } 
                    }
                }
                if(firstLaiziCardsNum >= 2 && secondLaiziCardsNum >= 2){
                    if(fourUseLaizi == 0 && firstLaiziCardsNum == 4){
                        std::vector<uint8_t> copyFour(four);
                        copyFour.emplace_back(firstLaiziCards[0]);
                        copyFour.emplace_back(firstLaiziCards[1]);
                        copyFour.emplace_back(firstLaiziCards[2]);
                        copyFour.emplace_back(firstLaiziCards[3]);
                        result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                    }else if(fourUseLaizi <= firstLaiziCardsNum && secondLaiziCardsNum == 4){
                        std::vector<uint8_t> copyFour(four);
                        copyFour.emplace_back(secondLaiziCards[0]);
                        copyFour.emplace_back(secondLaiziCards[1]);
                        copyFour.emplace_back(secondLaiziCards[2]);
                        copyFour.emplace_back(secondLaiziCards[3]);
                        result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                    }else if(firstLaiziCardsNum - fourUseLaizi >= 2 && secondLaiziCardsNum >= 2){
                        std::vector<uint8_t> copyFour(four);
                        copyFour.emplace_back(firstLaiziCards[firstLaiziCardsNum - 2]);
                        copyFour.emplace_back(firstLaiziCards[firstLaiziCardsNum - 1]);
                        copyFour.emplace_back(secondLaiziCards[0]);
                        copyFour.emplace_back(secondLaiziCards[1]);
                        result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                    }
                }
            }
        }
    }

    if(totalCardNum >= 6 ){
        //airplane without swings
        //从2 到 6 连续飞机
        for(int i = 2; i < 7; i++){
            int condition = 3 * i;
            if(condition > totalCardNum){
                break;
            }
            bool findAirplane = false;
            for(int j = 0; j < 13 - i; j++){
                std::vector<int> emptyParts;
                std::vector<uint8_t> airplane(condition);
                std::vector<int> emptyParts;
                for(int k = j; k < i + j; k++){
                    int kIndex = 3 * (k - j);
                    const auto& one = normalCards[k];
                    int oneNum = normalCardsNum[k];
                    if(oneNum >= 3){
                        airplane[kIndex + 0] = one[0]->origin;
                        airplane[kIndex + 1] = one[1]->origin;
                        airplane[kIndex + 2] = one[2]->origin;
                    }else if(oneNum == 2){
                        airplane[kIndex + 0] = one[0]->origin;
                        airplane[kIndex + 1] = one[1]->origin;
                        emptyParts.emplace_back(kIndex + 2);
                    }else if(oneNum == 1){
                        airplane[kIndex + 0] = one[0]->origin;
                        emptyParts.emplace_back(kIndex + 1);
                        emptyParts.emplace_back(kIndex + 2);
                    }else{
                        emptyParts.emplace_back(kIndex + 0);
                        emptyParts.emplace_back(kIndex + 1);
                        emptyParts.emplace_back(kIndex + 2);
                    }
                }
                int useLaiziNum = emptyParts.size();
                if(useLaiziNum <= laiziCardsNum){
                    findAirplane = true;
                    for(int k = 0; k < useLaiziNum; k++){
                        airplane[emptyParts[k]] = laiziCards[k]->origin;
                    } 
                    result.emplace_back(static_cast<Illegal>(static_cast<int>(Illegal::AIRPLANE_2_NOT_SWINGS) + i - 2),airplane);

                    if(totalCardNum >= condition + i){
                        
                    }
                }

            }
            if(!findAirplane){
                break;
            }
        }
    }
}
}

#endif//__DDZ_ALGORITHM_H__
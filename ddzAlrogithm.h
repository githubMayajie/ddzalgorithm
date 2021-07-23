#ifndef __DDZ_ALROGITHM_H__
#define __DDZ_ALROGITHM_H__

#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <tuple>
namespace ddzalgorithm{
//max hand card
#define DDZ_ALGORITHM_MAX_HANDLE_CARDS 20
//support space shuttle
// #define DDZ_ALGORITHM_USE_SHUTTLE
//think single has all card(include same cards)
// #define DDZ_ALGORITHM_SINGLE_HAS_ALL_CARDS
// 飞机中三条不能有2？参考wikipedia
#define DDZ_ALGORITHM_MAX_AIRPLANE 12

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
#ifdef USE_SHUTTLE
    SPACE_SHUTTLE_2_NOT_SWINGS                   = 0x80,//3333 4444
    SPACE_SHUTTLE_3_NOT_SWINGS                   = 0x81,//
    SPACE_SHUTTLE_4_NOT_SWINGS                   = 0x82,//16 cards

    SPACE_SHUTTLE_2_WITH_SWINGS                   = 0x83,//3333 4444 5678
    SPACE_SHUTTLE_3_WITH_SWINGS                   = 0x84,//16 cards
#endif
};

struct Token{
    Token(Illegal p1,std::vector<uint8_t> p2):
        type(p1),
        data(p2){}
    Illegal type;
    std::vector<uint8_t> data;
};

std::vector<Token> getToken(std::vector<uint8_t> input)
{
    //parse byte to card and get some info
    std::vector<Token> result;
    auto cards = input.size();
    std::vector<Card> parseCard(cards);
    for(const auto& one : input)
    {
        parseCard.emplace_back(one);
    }

    // get info            3 4 5 6 7,  8 9 10j q   k A 2
    // int cardNums[13] = {0,0,0,0,0,  0,0,0,0,0,  0,0,0};
    const Card* smallJoker = nullptr;
    const Card* bigJoker = nullptr;
    std::vector<const Card*> laiziVec; 
    std::vector<const Card*> normalVec[13];
    int normalNumVec[13];
    int maxNormalNum = 0;
    for(const auto& one: parseCard)
    {
        if(one.type == CardType::LAIZI){
            laiziVec.emplace_back(&one);
        }else if(one.type == CardType::SMALL_JOKER){
            smallJoker = &one;
        }else if(one.type == CardType::BIG_JOKER){
            bigJoker = &one;
        }else{
            normalVec[static_cast<int>(one.num)].emplace_back(&one);
        }
    }
    for(int i = 0; i < 13; i++){
        auto num = normalVec[i].size();
        normalNumVec[i] = num;
        if(num > maxNormalNum){
            maxNormalNum = num;
        }
    }
    // 双癞子（天地癞子）
    auto laiziNum = laiziVec.size();
    std::vector<Card*> firstLaiziVec;
    std::vector<Card*> secondLaiziVec;
    if(laiziNum > 0){
        const CardNum firstNum = laiziVec[0]->num;
        firstLaiziVec.emplace_back(laiziVec[0]);
        for(int i = 1; i < laiziNum; i++){
            if(firstNum == laiziVec[i]->num){
                firstLaiziVec.emplace_back(laiziVec[i]);
            }else{
                secondLaiziVec.emplace_back(laiziVec[i]);
            }
        }
    }

    //rocket
    if(smallJoker != nullptr && bigJoker != nullptr){
        std::vector<uint8_t> rocket{smallJoker->origin,bigJoker->origin};
        result.emplace_back(Illegal::ROCKET,rocket);
    }

    //bomb
    if(maxNormalNum + laiziNum >= 4){
        int illegal = static_cast<int>(Illegal::BOMB_8_PURE_JOKER);
        for(int i = 8; i >= 4; i++){
            //bomb pure joker
            if(i <= laiziNum){
                std::vector<uint8_t> bomb;
                for(int j = 0; j < i; j++){
                    bomb.emplace_back(laiziVec[j]->origin);
                }
                result.emplace_back(static_cast<Illegal>(illegal),bomb);
            }

            //bomb comb
            if(i <= maxNormalNum + laiziNum){
                for(int j = 0; j < i; j++){
                    // auto num = currentNumVec.size();
                    auto num = normalNumVec[j];
                    if(num > 0 && num + laiziNum >= i){
                        const auto& normal = normalVec[j];
                        //例如 22 8(*)88 可以组成2888 也可组成22 88
                        for(int useNormalNum = 1; useNormalNum <= num; useNormalNum++){
                            if(useNormalNum + laiziNum < i){
                                continue;
                            }
                            std::vector<uint8_t> bomb;
                            int j = 0;
                            for (j = 0; j < useNormalNum; j++){
                                bomb.emplace_back(normal[j]->origin);
                            }
                            for(int k = 0; k < i - useNormalNum; k++){
                                bomb.emplace_back(laiziVec[k]->origin);
                            }
                            //特殊情况，当没有使用癞子，则为正常炸弹
                            if(useNormalNum == 4 && i == 4){
                                result.emplace_back(Illegal::BOMB_4_WITHOU_JOKER,bomb);
                            }else{
                                result.emplace_back(static_cast<Illegal>(illegal - 1),bomb);
                            }
                        }
                    }
                }
            }
            illegal = illegal - 2;
        }
    }


    //single 
    if (cards >= 1){
        std::vector<uint8_t> tempSingles;
        for(const auto& currentNumVec: normalVec){
            auto num = currentNumVec.size();
            if(num > 0){
#ifdef DDZ_ALGORITHM_SINGLE_HAS_ALL_CARDS
                // every card be record
                for(int i = 0; i < num; i++){
                    tempSingles.emplace_back(currentNumVec[i]->origin);
                }
#else
                // same cards only first can be record
                tempSingles.emplace_back(currentNumVec[0]->origin);
#endif
            }
        }
        if(laiziNum > 0){
#ifdef DDZ_ALGORITHM_SINGLE_HAS_ALL_CARDS
            for(int i = 0; i < laiziNum; i++){
                tempSingles.emplace_back(laiziVec[i]->origin);
            }
#else
            // 癞子被当作单排，天地癞子各记录一张
            if(firstLaiziVec.size() > 0){
                tempSingles.emplace_back(firstLaiziVec[0]->origin);
            }
            if(secondLaiziVec.size() > 0){
                tempSingles.emplace_back(secondLaiziVec[0]->origin);
            }
#endif
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
    if(cards >= 2){
        for(int i = 0; i < 13; i++){
            int num = normalNumVec[i];
            if(num > 0 && num + laiziNum >= 2){
                std::vector<uint8_t> pair;
                const auto& one = normalVec[i];
                if(num == 1){
                    pair.emplace_back(one[0]->origin);
                    pair.emplace_back(laiziVec[0]->origin);
                }else{
                    //exclude same part(eg:222 only can be record one pair 22)
                    pair.emplace_back(one[0]->origin);
                    pair.emplace_back(one[1]->origin);
                }
                result.emplace_back(Illegal::PAIR,pair);
            }
        }
        // 癞子也可以作为对子，失去万能牌作用
        if(laiziNum >= 2){
            if(firstLaiziVec.size() >= 2){
                std::vector<uint8_t> pair{firstLaiziVec[0]->origin,firstLaiziVec[1]->origin};
                result.emplace_back(Illegal::PAIR,pair);
            }
            if(secondLaiziVec.size() >= 2){
                std::vector<uint8_t> pair{secondLaiziVec[0]->origin,secondLaiziVec[1]->origin};
                result.emplace_back(Illegal::PAIR,pair);
            }
        }
        //rocket is not pair
    }

    //triple 
    if(laiziNum + maxNormalNum >= 3){
        //1. 以normal牌为3条,先排除癞子做三条的可能，
        for(int i = 0; i < 13; i++){
            int num = normalNumVec[i];
            if(num > 0 && num + laiziNum >= 3){
                const auto& one = normalVec[i];
                std::vector<uint8_t> triple;
                int useLaiziNum = 0;
                if(num == 1){
                    triple.emplace_back(one[0]->origin);
                    triple.emplace_back(laiziVec[0]->origin);
                    triple.emplace_back(laiziVec[1]->origin);
                    useLaiziNum = 2;
                }else if(num == 2){
                    triple.emplace_back(one[0]->origin);
                    triple.emplace_back(one[1]->origin);
                    triple.emplace_back(laiziVec[0]->origin);
                    useLaiziNum = 1;
                }else{
                    triple.emplace_back(one[0]->origin);
                    triple.emplace_back(one[1]->origin);
                    triple.emplace_back(one[2]->origin);
                }
                result.emplace_back(Illegal::TRIPLE_PURE,triple);

                //triple with one
                if(cards >= 4){
                    std::vector<uint8_t> tempSingles;
                    for(int j = 0; j < 13; j++){
                        if(normalNumVec[j] > 0 && j != i){
                            tempSingles.emplace_back(normalVec[j][0]->origin);
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
                    // 考虑癞子特殊情况，
                    if(useLaiziNum == 0){
                        // 1. 当没有使用癞子，这时候天地癞子都可以做单牌
                        if(firstLaiziVec.size() > 0){
                            tempSingles.emplace_back(firstLaiziVec[0]->origin);
                        }
                        if(secondLaiziVec.size() > 0){
                            tempSingles.emplace_back(secondLaiziVec[0]->origin);
                        }
                        calcTripleSingle();
                    }else if(useLaiziNum == laiziNum){
                        // 2.所有的癞子都使用完了，这时候不可能做单牌
                        calcTripleSingle();
                    }else{
                        // 3.还有癞子没有使用
                        if(firstLaiziVec.size() > 0 && secondLaiziVec.size() > 0){
                            // 当存在天地癞子的情况，需要重组
                            auto calcTripleSingle2 = [&](const std::vector<Card*>& first,const std::vector<Card*>& second){
                                std::vector<uint8_t> copyTriple(triple);
                                std::vector<uint8_t> tempSingles2(tempSingles);
                                std::vector<uint8_t> priorityLaiZi;
                                for(const auto& laiziCard : first){
                                    priorityLaiZi.emplace_back(laiziCard->origin);
                                }
                                for(const auto& laiziCard : second){
                                    priorityLaiZi.emplace_back(laiziCard->origin);
                                }
                                int index = 0;
                                for(int i = 3 - useLaiziNum; i < 3; i++){
                                    copyTriple[i] = priorityLaiZi[index++];
                                }
                                tempSingles2.emplace_back(second[second.size() - 1]->origin);
                                for(const auto& one: tempSingles2){
                                    std::vector<uint8_t> copyTriple2(copyTriple);
                                    copyTriple2.emplace_back(one);
                                    result.emplace_back(Illegal::TRIPLE_WITH_SINGLE,copyTriple2);
                                }
                            };
                            calcTripleSingle2(firstLaiziVec,secondLaiziVec);
                            calcTripleSingle2(secondLaiziVec,firstLaiziVec);
                        }else{
                            // 只有一种癞子，没必要重组,把癞子作为单牌
                            tempSingles.emplace_back(laiziVec[useLaiziNum]->origin);
                            calcTripleSingle();
                        }
                    }
                }

                //triple with pair
                int leftLaizi = laiziNum - useLaiziNum;
                if(maxNormalNum + leftLaizi >= 2 && cards >= 5){
                    std::vector<std::tuple<uint8_t,uint8_t>> pairs;
                    for(int j = 0; j < 13; j++){
                        int num2 = normalNumVec[j];
                        if(num2 > 0 && i != j && num2 + leftLaizi >= 2){
                            const auto& one = normalVec[j];
                            if(num2 == 1){
                                pairs.emplace_back(one[0]->origin,laiziVec[useLaiziNum]->origin);
                            }else{
                                pairs.emplace_back(one[0]->origin,one[1]->origin);
                            }
                        }
                    }
                    for(const auto& one : pairs){
                        std::vector<uint8_t> copyTriple(triple);
                        copyTriple.emplace_back(std::get<0>(one));
                        copyTriple.emplace_back(std::get<1>(one));
                        result.emplace_back(Illegal::TRIPLE_WITH_PAIR,copyTriple);
                    }
                    //已经使用了不知道多少个癞子，组合关系过于复杂,需要重置癞子顺序
                    if(leftLaizi >= 2){
                        auto calcTriplePair = [&](const std::vector<Card*>& firstVec,const std::vector<Card*>& secondVec){
                            std::vector<uint8_t> copyTriple(triple);
                            std::vector<Card*> priorityLaiZi;
                            for(const auto& laiziCard : firstVec){
                                priorityLaiZi.emplace_back(laiziCard);
                            }
                            for(const auto& laiziCard : secondVec){
                                priorityLaiZi.emplace_back(laiziCard);
                            }
                            int index = 0;
                            for(int i = 3 - useLaiziNum;i < 3; i--){
                                const auto& card = priorityLaiZi[index++];
                                copyTriple[i] = card->origin;
                            }
                            CardNum desireCardNum = secondVec[0]->num;
                            CardNum lastTwoOfCard = priorityLaiZi[laiziNum - 2]->num;
                            CardNum lastFirstOfCard = priorityLaiZi[laiziNum - 2]->num;
                            //最后两个一定要和期望的匹配
                            if (desireCardNum == lastTwoOfCard && desireCardNum == lastFirstOfCard){
                                copyTriple[4] = priorityLaiZi[laiziNum - 2]->origin;
                                copyTriple[5] = priorityLaiZi[laiziNum - 1]->origin;
                                result.emplace_back(Illegal::TRIPLE_WITH_PAIR,copyTriple);
                            } 
                        };
                        //重组癞子优先级，
                        if(firstLaiziVec.size() >= 2){
                            calcTriplePair(secondLaiziVec,firstLaiziVec);
                        }
                        if(secondLaiziVec.size() >= 2){
                            calcTriplePair(firstLaiziVec,secondLaiziVec);
                        }
                        
                    }
                }                
            }
        }

        //以癞子作为3条
        if(laiziNum > 3){
            auto calcLaiziTriple = [&](const std::vector<Card*> firstVec, const std::vector<Card*> secondVec){
                int canUserLaiziNum = secondVec.size();
                std::vector<uint8_t> triple{firstVec[0]->origin,firstVec[1]->origin,firstVec[2]->origin};
                result.emplace_back(Illegal::TRIPLE_PURE,triple);
                if(cards >= 4){
                    std::vector<uint8_t> tempSingles;
                    for(int i = 0; i < 13; i++){
                        if(normalNumVec[i] > 0){
                            tempSingles.emplace_back(normalVec[i][0]->origin);
                        }
                    }
                    if(canUserLaiziNum > 0){
                        tempSingles.emplace_back(secondVec[0]->origin);
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
                int leftLaiziNum = laiziNum - 3;
                if(cards >= 5){
                    std::vector<std::tuple<uint8_t,uint8_t>> pairs;
                    for(int i = 0; i <13; i++){
                        int num = normalNumVec[i]; 
                        if(num > 0 && num + canUserLaiziNum >= 2){
                            if(num == 1){
                                pairs.emplace_back(normalVec[i][0]->origin,secondVec[0]->origin);
                            }else{
                                pairs.emplace_back(normalVec[i][0]->origin,normalVec[i][1]->origin);
                            }
                        }
                        //3(*)333 4(*) 8 不知道可以组成 333 4(*)8 ,也可以组成 333 3(*)8 队吗？
                        if(num == 1 && num + canUserLaiziNum >=2 && firstLaiziVec.size() == 4){
                            pairs.emplace_back(normalVec[i][0]->origin,firstLaiziVec[3]->origin);
                        }
                    }
                    if(canUserLaiziNum >= 1){
                        if(firstLaiziVec.size() == 4){
                            //癞子second失去通用功能，当作普通牌，而癞子1还有剩余，
                            //3(*)333 5(*)不知道可以组成333 55队吗？
                            pairs.emplace_back(secondVec[0]->origin,firstLaiziVec[3]->origin);
                        }
                    }
                    if(canUserLaiziNum >= 2){
                        pairs.emplace_back(secondVec[0]->origin,secondVec[1]->origin);
                    }

                    if(smallJoker != nullptr && canUserLaiziNum > 0){
                        pairs.emplace_back(smallJoker->origin,secondVec[0]->origin);
                    }
                    if(bigJoker != nullptr && canUserLaiziNum > 0){
                        pairs.emplace_back(bigJoker->origin,secondVec[0]->origin);
                    }
                    for(const auto& one : pairs){
                        std::vector<uint8_t> copyTriple(triple);
                        copyTriple.emplace_back(std::get<0>(one));
                        copyTriple.emplace_back(std::get<1>(one));
                        result.emplace_back(Illegal::TRIPLE_WITH_PAIR,copyTriple);
                    }
                }
            };
            if(firstLaiziVec.size() >= 3){
                calcLaiziTriple(firstLaiziVec,secondLaiziVec);
            }
            if(secondLaiziVec.size() >= 3){
                calcLaiziTriple(secondLaiziVec,firstLaiziVec);
            }
        }
    }

    if(cards >= 5){
        //最少5链，最多12链
        for(int i = 5; i < 13; i++){ //5 - 12
            if(i > cards){
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
                    if(normalNumVec[k] > 0){
                        chains[kIndex] = normalVec[k][0]->origin;
                    }else{
                        emptyParts.emplace_back(kIndex);
                    }
                }
                // 加上癞子组合
                int needLaiziNum = emptyParts.size();
                if(needLaiziNum <= laiziNum){
                    for(int k = 0; k < needLaiziNum; k++){
                        chains[emptyParts[k]] = laiziVec[k]->origin;
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

    if(cards >= 6){
        //pair chain
        //最少3链，最多10链
        for(int i = 3; i < 11; i++){ //3 - 10
            int needCardNum = 2 * i;
            if(needCardNum > cards){
                break;
            }
            bool hasChains = false;
            //j表示i链条从哪里开始
            for(int j = 0; j < 13 - i; j++){ //0 ~ 8 0 ~ 3
                std::vector<uint8_t> chains(needCardNum);
                std::vector<int> emptyParts;

                // 获取链条的数据 从j开始的i链条
                for(int k = j; k < i + j; k++){
                    const auto& one = normalVec[k];
                    int oneNum = normalNumVec[k];
                    //kIndex表示chains中的位置
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
                // 加上癞子组合
                int needLaiziNum = emptyParts.size();
                if(needLaiziNum <= laiziNum){
                    for(int k = 0; k < needLaiziNum; k++){
                        chains[emptyParts[k]] = laiziVec[k]->origin;
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

    if(cards >= 6 ){
        // four with two singles and two pairs
        for(int i = 0; i < 13; i++){
            int num = normalNumVec[i];
            if(num > 0 && num + laiziNum >= 4){
                const auto& one = normalVec[i];
                std::vector<uint8_t> four;
                int useLaiziNum = 0;
                if(num == 1){
                    four.emplace_back(one[0]->origin);
                    four.emplace_back(laiziVec[0]->origin);
                    four.emplace_back(laiziVec[1]->origin);
                    four.emplace_back(laiziVec[2]->origin);
                    useLaiziNum = 3;
                }else if(num == 2){
                    four.emplace_back(one[0]->origin);
                    four.emplace_back(one[1]->origin);
                    four.emplace_back(laiziVec[0]->origin);
                    four.emplace_back(laiziVec[1]->origin);
                    useLaiziNum = 2;
                }else if(num == 3){
                    four.emplace_back(one[0]->origin);
                    four.emplace_back(one[1]->origin);
                    four.emplace_back(one[2]->origin);
                    four.emplace_back(laiziVec[0]->origin);
                    useLaiziNum = 1;
                }else{
                    four.emplace_back(one[0]->origin);
                    four.emplace_back(one[1]->origin);
                    four.emplace_back(one[2]->origin);
                    four.emplace_back(one[3]->origin);
                    useLaiziNum = 0;
                }

                //with two singles
                std::vector<const Card*> tempSingles;
                for(int j = 0; j < 13; j++){
                    if(j != i && normalNumVec[j] > 0){
                        for(const auto& card : normalVec[j]){
                            tempSingles.emplace_back(card);
                        }
                    }
                }
                if(smallJoker != nullptr){
                    tempSingles.emplace_back(smallJoker);
                }
                if(bigJoker != nullptr){
                    tempSingles.emplace_back(bigJoker);
                }
                //唯一编码，去除重复
                unsigned char record[16 * 16 + 16];
                std::memset(record,'\0',16 * 16 + 16);
                auto calcFourWithTwo = [&](std::vector<uint8_t> p1,std::vector<const Card*> p2){
                    auto p2Size = p2.size();
                    if(p2Size >= 2){
                        //移除相似的部分,冒泡算法遍历组合
                        for(int i = 0; i < p2Size; i++){
                            for(int j = i + 1; j < p2Size; j++){
                                const Card* first = p2[i];
                                const Card* second = p2[j];
                                int firstKey = first->getRecordCode();
                                int secondKey = second->getRecordCode();
                                int key1 = firstKey * 16 + secondKey;
                                int key2 = secondKey * 16 + firstKey;
                                if(record[key1] == '\0' && record[key2] == '\0'){
                                    record[key1] = '\1';
                                    record[key2] = '\1';
                                    std::vector<uint8_t> copyFour(p1);
                                    copyFour.emplace_back(first->origin);
                                    copyFour.emplace_back(second->origin);
                                    result.emplace_back(Illegal::FOUR_WITH_TWO_SINGLES,copyFour);
                                }
                            }
                        }
                    }
                };
                //不影响大小，所以不考虑癞子的顺序组合问题
                for(int i = useLaiziNum; i < laiziNum; i++){
                    tempSingles.emplace_back(laiziVec[i]);
                }
                calcFourWithTwo(four,tempSingles);
                // if(useLaiziNum == laiziNum){
                //     calcFourWithTwo(four,tempSingles);
                // }else{
                //     // 癞子还有剩余，需要重新组合
                //     if(firstLaiziVec.size() == 0 || secondLaiziVec.size() == 0){
                //         //只有一种癞子，直接把剩余癞子加入即可(癞子有可能为对子，必须全部加入)
                //         for(int i = useLaiziNum; i < laiziNum; i++){
                //             tempSingles.emplace_back(laiziVec[i]);
                //         }
                //         calcFourWithTwo(four,tempSingles);
                //     }else{
                //         calcFourWithTwo(four,tempSingles);
                //         //aa bbb(*) ccc(*) 可以组成 aabb bc aabbcc aacc bb
                //         //交叉
                //     }
                // }

                //with two pairs
                if(cards >= 8){
                    for(int j = 0; j < 13; j++){
                        int firstNum = normalNumVec[j];
                        int leftLaiziNum = laiziNum - useLaiziNum;
                        if(firstNum > 0 && i != j && firstNum + leftLaiziNum >= 2){
                            const auto& one = normalVec[j];
                            std::vector<uint8_t> first(2);
                            first.emplace_back(one[0]->origin);
                            int useLaiziNum2 = 0;
                            if(firstNum == 1){
                                first.emplace_back(laiziVec[useLaiziNum + useLaiziNum2]->origin);
                                ++useLaiziNum2;
                            }else{
                                first.emplace_back(one[1]->origin);
                            }

                            //find second pair
                            leftLaiziNum = leftLaiziNum - useLaiziNum2;
                            for(int z = j + 1; z < 13; z++){
                                int secondNum = normalNumVec[j];
                                if(z != i && secondNum > 0 && secondNum + leftLaiziNum >= 2){
                                    const auto& two = normalVec[z];
                                    std::vector<uint8_t> second(2);
                                    second.emplace_back(two[0]->origin);
                                    if(secondNum == 1){
                                        second.emplace_back(laiziVec[useLaiziNum + useLaiziNum2]->origin);
                                    }else{
                                        second.emplace_back(two[1]->origin);
                                    }
                                    std::vector<uint8_t> copyFour(four);
                                    copyFour.emplace_back(first[0]);
                                    copyFour.emplace_back(first[1]);
                                    copyFour.emplace_back(second[0]);
                                    copyFour.emplace_back(second[1]);
                                    result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                                }
                            }
                            if(leftLaiziNum >= 2){
                                std::vector<uint8_t> copyFour(four);
                                //建立优先级队列，替换之前的癞子,不用考虑顺序，只找到可以匹配的一种即可
                                error(2)
                                copyFour.emplace_back(first[0]);
                                copyFour.emplace_back(first[1]);
                                copyFour.emplace_back(laiziVec[useLaiziNum + useLaiziNum2]->origin);
                                copyFour.emplace_back(laiziVec[useLaiziNum + useLaiziNum2 + 1]->origin);
                                result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                            }
                        }
                    }
                }
            }
        }
    }

    if(cards >= 6 ){
        //airplane without swings
        //从2 到 6 连续飞机
        for(int i = 2; i < 7; i++){
            int condition = 3 * i;
            if(condition > cards){
                break;
            }
            //从j开始查询 到ddz_algorithm_max_airplane-i 结束
            for(int j = 0; j < DDZ_ALGORITHM_MAX_AIRPLANE - i; j++){
                std::vector<int> emptyParts;
                int useLaiziNum = 0;

                std::vector<uint8_t> airplane(condition);
                std::vector<int> emptyParts;
                //从j开始查询i连续的飞机
                for(int k = j; k < i + j; k++){
                    int kIndex = 3 * (k - j);
                    const auto& one = normalVec[k];
                    int oneNum = normalNumVec[k];
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
                int needLaiziNum = emptyParts.size();
                bool success = false;
                if(needLaiziNum <= laiziNum){
                    for(int k = 0; k < needLaiziNum; k++){
                        airplane[emptyParts[k]] = laiziVec[k]->origin;
                    } 
                    result.emplace_back(static_cast<Illegal>(static_cast<int>(Illegal::AIRPLANE_2_NOT_SWINGS) + i - 2),airplane);
                    success = true;
                }

                if(success){
                    useLaiziNum = needLaiziNum;
                    std::vector<const Card*> singles;
                    for(int k = 0; k < 13; k++){
                        for(const auto& one : normalVec[k]){
                            if(std::find(airplane.begin(),airplane.end(),one->origin) == airplane.end()){
                                singles.emplace_back(&one);
                            }
                        }
                    }
                    if(smallJoker != nullptr){
                        singles.emplace_back(smallJoker);
                    }
                    if(bigJoker != nullptr){
                        singles.emplace_back(bigJoker);
                    }
                    int leftLaizi = laiziNum - useLaiziNum;
                    if(condition + i <= cards){
                        //choose i from singles and leftLaizi
                        if(i <= leftLaizi + singles.size()){
                            
                        }
                    }
                    
                    
                }
            } 
        }


         // AIRPLANE_2_WITH_SMALL_SWINGS
        for(int i = 2; i < 6; i++){
            int condition = 3 * i;
            for(int j = 0; j < MAX_AIRPLANE - i; j++){
                std::vector<int> emptyParts;
                int laiziNum2 = laiziNum;
                int useLaiziNum = 0;
                int alreadNum = 0;
                for(int k = 0; k < i; k++){
                    alreadNum += normalNumVec[k];
                }
                if(alreadNum + laiziNum2 >= condition){
                    std::vector<uint8_t> airplane(condition);
                    for(int k = 0; k < i; k++){
                        int existNum = normalNumVec[k];
                        int z = 0;
                        for(z = 0; z < std::min(3,existNum);z++){
                            airplane.emplace_back(normalVec[k][z]->origin);
                        }
                        while((z++) < 3){
                            airplane.emplace_back(laiziVec[useLaiziNum++]->origin);
                        };
                    }

                    //with two singles
                    int index2 = 0;
                    std::vector<const Card*> singles;
                    for(const auto& currentNumVec2: normalVec){
                        if((index2++) != j){
                            if(currentNumVec2.size() > 0){
                                singles.emplace_back(currentNumVec2[0]);
                            }
                        }
                    }
                    //joker as single
                    if(smallJoker != nullptr){
                        singles.emplace_back(smallJoker);
                    }
                    if(bigJoker != nullptr){
                        singles.emplace_back(bigJoker);
                    }
                    //laizi as single
                    if(useLaizNum < laiziNum){
                        for(int i = 0; i < laiziNum - useLaizNum; i++){
                            singles.emplace_back(laiziVec[i + useLaizNum]);
                        }
                    }
                    //唯一编码
                    unsigned char record[16 * 16 + 16];
                    std::memset(record,'\0',16 * 16 + 16);
                    if(singles.size() >= 2){
                        //移除相似的部分
                        for(int i = 0; i < singles.size(); i++){
                            for(int j = i + j;j < singles.size(); j++){
                                const Card* first = singles[i];
                                const Card* second = singles[j];
                                int key = first->getRecordCode() * 16 + second->getRecordCode();
                                if(record[key] == '\0'){
                                    record[key] = '\1';
                                    std::vector<uint8_t> copyFour(four);
                                    copyFour.emplace_back(first->origin);
                                    copyFour.emplace_back(second->origin);
                                    result.emplace_back(Illegal::FOUR_WITH_TWO_SINGLES,copyFour);
                                }
                            }
                        }
                    }

                    
                    result.emplace_back(static_cast<Illegal>(static_cast<int>(Illegal::AIRPLANE_2_WITH_SMALL_SWINGS) + i - 2),airplane);
                }
            } 
        }



    }


    


}


}



#endif//__DDZ_ALGORITHM_H__
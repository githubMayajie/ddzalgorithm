#ifndef __DDZ_ALROGITHM_H__
#define __DDZ_ALROGITHM_H__

#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
namespace ddzalgorithm{
#define MAX_HANDLE_CARDS 20
#define USE_SHUTTLE

enum class CardNum{
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
    int getRecordCode() const{
        if(type == CardType::SMALL_JOKER){
            return 0xd;
        }else if(type == CardType::BIG_JOKER){
            return 0xe;
        }
        //普通牌和癞子不受type影响
        return static_cast<int>(num);
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

    CHAIN_5                     = 0x40,
    CHAIN_6                     = 0x41,
    CHAIN_7                     = 0x42,
    CHAIN_8                     = 0x43,
    CHAIN_9                     = 0x44,
    CHAIN_10                    = 0x45,
    CHAIN_11                    = 0x46,
    CHAIN_12                    = 0x47,          //3 ~ ACE 12cards

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

    //sort
    // std::sort(parseCard.begin(),parseCard.end(),[](const Card& left, const Card& right)->bool{
    //     return left.num < right.num;
    // });
    
    //get info             3 4 5 6 6,  8 9 10j q   k A 2
    // int cardNums[13] = {0,0,0,0,0,  0,0,0,0,0,  0,0,0};
    const Card* smallJoker = nullptr;
    const Card* bigJoker = nullptr;
    //癞子,癞子效果都一样，不用在区分如何组合癞子
    std::vector<const Card*> laiziVec; 
    std::vector<const Card*> normalVec[13];
    int normalNumVec[13];

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
        normalNumVec[i] = normalVec[i].size();
    }

    //rocket
    if(smallJoker != nullptr && bigJoker != nullptr){
        std::vector<uint8_t> rocket{smallJoker->origin,bigJoker->origin};
        result.emplace_back(Illegal::ROCKET,rocket);
    }

    //bomb
    auto laiziNum = laiziVec.size();
    int lastMaxNum = 0;
    for(const auto& one: normalVec){
        auto num = one.size();
        if(num > lastMaxNum){
            lastMaxNum = num;
        }
    }

    // all boom prev condition
    if(lastMaxNum + laiziNum >= 4){
        int illegal = static_cast<int>(Illegal::BOMB_8_PURE_JOKER);
        for(int i = 8; i >= 4; i++){
            //bomb pure joker
            //纯癞子炸弹通过计算纯癞子个数组合
            if(laiziNum >= i){
                std::vector<uint8_t> bomb;
                for(int j = 0; j < i; j++){
                    bomb.emplace_back(laiziVec[j]->origin);
                }
                result.emplace_back(static_cast<Illegal>(illegal),bomb);
            }

            //bomb comb
            //非纯癞子炸弹 
            //计算最大满足条件
            //根据使用的正常card个数来生成炸弹组合，例如 22 888 可以组成2888 也可组成22 88
            //特殊情况，当没有使用癞子，则为正常炸弹
            for(const auto& currentNumVec: normalVec){
                auto num = currentNumVec.size();
                if(num > 0 && num + laiziNum >= i){
                    for(int useNormalNum = 1; useNormalNum <= num; useNormalNum++){
                        if(useNormalNum + laiziNum < i){
                            continue;
                        }
                        std::vector<uint8_t> bomb;
                        int j = 0;
                        for (j = 0; j < useNormalNum; j++){
                            bomb.emplace_back(currentNumVec[j]->origin);
                        }
                        for(int k = 0; k < i - useNormalNum; k++){
                            bomb.emplace_back(laiziVec[k]->origin);
                        }
                        if(useNormalNum == 4 && i == 4){
                            result.emplace_back(Illegal::BOMB_4_WITHOU_JOKER,bomb);
                        }else{
                            result.emplace_back(static_cast<Illegal>(illegal - 1),bomb);
                        }
                    }
                }
            }
            illegal = illegal - 2;
        }
    }


    //single 
    if (cards >= 1){
        for(const auto& currentNumVec: normalVec){
            auto num = currentNumVec.size();
            if(num > 0){
                //只记录一次，例如 22 3 4 5 会被当做2 3 4 5当做单牌，至于2只使用第一个，看后续是否需要修改
                std::vector<uint8_t> single{currentNumVec[0]->origin};
                result.emplace_back(Illegal::SINGLE,single);
            }
        }
        if(laiziNum >= 8){
            //记录一次癞子
            std::vector<uint8_t> single{laiziVec[0]->origin};
            result.emplace_back(Illegal::SINGLE,single);
        }
        if(smallJoker != nullptr){
            std::vector<uint8_t> single{smallJoker->origin};
            result.emplace_back(Illegal::SINGLE,single);
        }
        if(bigJoker != nullptr){
            std::vector<uint8_t> single{bigJoker->origin};
            result.emplace_back(Illegal::SINGLE,single);
        }
    }

    //pair
    if(cards >= 2){
        //双王不是 pair 
        for(int i = 0; i < 13; i++){
            int num = normalNumVec[i];
            if(num > 0 && num + laiziNum >= 2){
                std::vector<uint8_t> pair;
                const auto& one = normalVec[i];
                if(num == 1){
                    pair.emplace_back(one[0]->origin);
                    pair.emplace_back(laiziVec[0]->origin);
                }else{
                    pair.emplace_back(one[0]->origin);
                    pair.emplace_back(one[1]->origin);
                }
                result.emplace_back(Illegal::PAIR,pair);
            }
        }
        // //癞子不能单独打
        // if(laiziNum >= 2){
        //     std::vector<uint8_t> pair{laiziVec[0]->origin,laiziVec[1]->origin};
        //     result.emplace_back(Illegal::PAIR,pair);
        // }
    }

    //triple 
    if(cards >= 3){
        //triple pure
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
                    for(int j = 0; j < 13; j++){
                        if(normalNumVec[j] > 0 && j != i){
                            std::vector<uint8_t> copyTriple(triple);
                            copyTriple.emplace_back(normalVec[j][0]->origin);
                            result.emplace_back(Illegal::TRIPLE_WITH_SINGLE,copyTriple);
                        }
                    }
                    //laizi as single
                    if(useLaiziNum < laiziNum){
                        std::vector<uint8_t> copyTriple(triple);
                        copyTriple.emplace_back(laiziVec[useLaiziNum]->origin);
                        result.emplace_back(Illegal::TRIPLE_WITH_SINGLE,copyTriple);
                    }
                    //joker as single
                    if(smallJoker != nullptr){
                        std::vector<uint8_t> copyTriple(triple);
                        copyTriple.emplace_back(smallJoker->origin);
                        result.emplace_back(Illegal::TRIPLE_WITH_SINGLE,copyTriple);
                    }
                    if(bigJoker != nullptr){
                        std::vector<uint8_t> copyTriple(triple);
                        copyTriple.emplace_back(bigJoker->origin);
                        result.emplace_back(Illegal::TRIPLE_WITH_SINGLE,copyTriple);
                    }
                }

                //triple with pair
                if(cards >= 5){
                    int leftLaizi = laiziNum - useLaiziNum;
                    for(int j = 0; j < 13; j++){
                        int num2 = normalNumVec[j];
                        if(num2 > 0 && i != j && num2 + leftLaizi >= 2){
                            std::vector<uint8_t> copyTriple(triple);
                            const auto& one = normalVec[j];
                            copyTriple.emplace_back(one[0]->origin);
                            if(num2 == 1){
                                copyTriple.emplace_back(laiziVec[useLaiziNum]->origin);
                            }else{
                                copyTriple.emplace_back(one[1]->origin);
                            }
                            result.emplace_back(Illegal::TRIPLE_WITH_PAIR,copyTriple);
                        }
                    }
                    //laizi as pair
                    if(laiziNum - useLaiziNum >= 2){
                        std::vector<uint8_t> copyTriple(triple);
                        copyTriple.emplace_back(laiziVec[useLaiziNum]->origin);
                        copyTriple.emplace_back(laiziVec[useLaiziNum + 1]->origin);
                        result.emplace_back(Illegal::TRIPLE_WITH_PAIR,copyTriple);
                    }
                }                
            }
        }
        // 癞子不能单独打出去
        // if(laiziNum >= 3){
        //     std::vector<uint8_t> triple{laiziVec[0]->origin,laiziVec[1]->origin,laiziVec[2]->origin};
        //     result.emplace_back(Illegal::TRIPLE_PURE,triple);
        // }
    }

    if(cards >= 5){
        //最少5链，最多12链
        for(int i = 5; i < 13; i++){ //5 - 12
            if(i > cards){
                break;
            }

            //表示需要遍历的个数 j表示链条开始到结束
            for(int j = 0; j < 13 - i; j++){ //0 ~ 8 0 ~ 1
                std::vector<uint8_t> chains(i);
                std::vector<int> emptyParts;

                // 获取链条的数据
                for(int k = j; k < i; k++){
                    int kIndex = k - j;
                    if(normalNumVec[k] > 0){
                        chains[kIndex] = normalVec[i][0]->origin;
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
                }
            }
        }
    }

    if(cards >= 6){
        //pair chain
        //最少3链，最多10链
        for(int i = 3; i < 11; i++){ //3 - 10
            if(2 * i > cards){
                break;
            }

            //表示需要遍历的个数（j表示链条开始到结束
            for(int j = 0; j < 13 - i; j++){ //0 ~ 8 0 ~ 3
                std::vector<uint8_t> chains(i * 2);
                std::vector<int> emptyParts;

                // 获取链条的数据
                for(int k = j; k < i; k++){
                    const auto& one = normalVec[k];
                    int kIndex = 2 * (k - j);
                    if(normalNumVec[k] > 1){
                        chains[kIndex] = one[0]->origin;
                        chains[kIndex + 1] = one[1]->origin;
                    }else if(normalNumVec[k] > 0){
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
                }
            }
        }
    }

    if(true && cards >= 6 ){
        //four with two single
        int index = 0;
        for(const auto& currentNumVec: normalVec){
            ++index;
            auto num = currentNumVec.size();
            if(num > 0 && num + laiziNum >= 4){
                std::vector<uint8_t> four;
                int useLaizNum = 0;
                if(num == 1){
                    four.emplace_back(currentNumVec[0]->origin);
                    four.emplace_back(laiziVec[0]->origin);
                    four.emplace_back(laiziVec[1]->origin);
                    four.emplace_back(laiziVec[2]->origin);
                    useLaizNum = 3;
                }else if(num == 2){
                    four.emplace_back(currentNumVec[0]->origin);
                    four.emplace_back(currentNumVec[1]->origin);
                    four.emplace_back(laiziVec[0]->origin);
                    four.emplace_back(laiziVec[1]->origin);
                    useLaizNum = 2;
                }else if(num == 3){
                    four.emplace_back(currentNumVec[0]->origin);
                    four.emplace_back(currentNumVec[1]->origin);
                    four.emplace_back(currentNumVec[2]->origin);
                    four.emplace_back(laiziVec[0]->origin);
                    useLaizNum = 1;
                }else{
                    four.emplace_back(currentNumVec[0]->origin);
                    four.emplace_back(currentNumVec[1]->origin);
                    four.emplace_back(currentNumVec[2]->origin);
                    four.emplace_back(currentNumVec[3]->origin);
                    useLaizNum = 0;
                }

                //with two singles
                int index2 = 0;
                std::vector<const Card*> singles;
                for(const auto& currentNumVec2: normalVec){
                    ++index2;
                    if(index2 != index){
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
            }
        }

        //foru with two pair
        for(const auto& currentNumVec: normalVec){
            auto num = currentNumVec.size();
            if(num > 0 && num + laiziNum >= 4){
                std::vector<uint8_t> four;
                int useLaizNum = 0;
                if(num == 1){
                    four.emplace_back(currentNumVec[0]->origin);
                    four.emplace_back(laiziVec[0]->origin);
                    four.emplace_back(laiziVec[1]->origin);
                    four.emplace_back(laiziVec[2]->origin);
                    useLaizNum = 3;
                }else if(num == 2){
                    four.emplace_back(currentNumVec[0]->origin);
                    four.emplace_back(currentNumVec[1]->origin);
                    four.emplace_back(laiziVec[0]->origin);
                    four.emplace_back(laiziVec[1]->origin);
                    useLaizNum = 2;
                }else if(num == 3){
                    four.emplace_back(currentNumVec[0]->origin);
                    four.emplace_back(currentNumVec[1]->origin);
                    four.emplace_back(currentNumVec[2]->origin);
                    four.emplace_back(laiziVec[0]->origin);
                    useLaizNum = 1;
                }else{
                    four.emplace_back(currentNumVec[0]->origin);
                    four.emplace_back(currentNumVec[1]->origin);
                    four.emplace_back(currentNumVec[2]->origin);
                    four.emplace_back(currentNumVec[3]->origin);
                    useLaizNum = 0;
                }

                //find first pair
                unsigned char record[16 * 16 + 16];
                std::memset(record,'\0',16 * 16 + 16);
                for(int i = 0; i < 13; i++){
                    int firstNum = normalNumVec[i];
                    int leftLaiziNum = laiziNum - useLaizNum;
                    int useLaiziNum2 = 0;
                    std::vector<uint8_t> first(2);
                    if(firstNum > 0 && firstNum + leftLaiziNum >= 2){
                        if(firstNum == 1){
                            first.emplace_back(normalVec[i][0]->origin);
                            first.emplace_back(laiziVec[useLaizNum + useLaiziNum2]->origin);
                            ++useLaiziNum2;
                        }else{
                            //firstNum >= 2
                            first.emplace_back(normalVec[i][0]->origin);
                            first.emplace_back(normalVec[i][1]->origin);
                        }

                        //find second pair
                        for(int j = i + 1; j < 13; j++){
                            int secondNum = normalNumVec[j];
                            std::vector<uint8_t> second(2);
                            if(secondNum > 0 && secondNum + (leftLaiziNum - useLaiziNum2) >= 2){
                                if(secondNum == 1){
                                    second.emplace_back(normalVec[j][0]->origin);
                                    second.emplace_back(laiziVec[useLaizNum + useLaiziNum2]->origin);
                                }else{
                                    //secondNum >= 2
                                    second.emplace_back(normalVec[j][0]->origin);
                                    second.emplace_back(normalVec[j][1]->origin);
                                }
                                std::vector<uint8_t> copyFour(four);
                                copyFour.emplace_back(first[0]);
                                copyFour.emplace_back(first[1]);
                                copyFour.emplace_back(second[0]);
                                copyFour.emplace_back(second[1]);
                                result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                            }
                        }
                        if(leftLaiziNum - useLaiziNum2 >= 2){
                            std::vector<uint8_t> copyFour(four);
                            copyFour.emplace_back(first[0]);
                            copyFour.emplace_back(first[1]);
                            copyFour.emplace_back(laiziVec[useLaizNum + useLaiziNum2]->origin);
                            copyFour.emplace_back(laiziVec[useLaizNum + useLaiziNum2 + 1]->origin);
                            result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                        }
                    }
                }

                //纯癞子组成两队
                if(laiziNum - useLaizNum >= 4){
                    std::vector<uint8_t> copyFour(four);
                    copyFour.emplace_back(laiziVec[useLaizNum]->origin);
                    copyFour.emplace_back(laiziVec[useLaizNum + 1]->origin);
                    copyFour.emplace_back(laiziVec[useLaizNum + 2]->origin);
                    copyFour.emplace_back(laiziVec[useLaizNum + 3]->origin);
                    result.emplace_back(Illegal::FOUR_WITH_TWO_PAIRS,copyFour);
                }
            }
        }
    }


#define MAX_AIRPLANE 12
// #define MAX_AIRPLANE 13 斗地主不能带feiji
    if(true && cards >= 6 ){
        //airplane
        // AIRPLANE_2_NOT_SWINGS
        for(int i = 2; i < 7; i++){
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
                    result.emplace_back(static_cast<Illegal>(static_cast<int>(Illegal::AIRPLANE_2_NOT_SWINGS) + i - 2),airplane);
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
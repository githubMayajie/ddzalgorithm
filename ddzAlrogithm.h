#ifndef __DDZ_ALROGITHM_H__
#define __DDZ_ALROGITHM_H__

#include <cstdint>
#include <vector>
#include <algorithm>
namespace ddzalgorithm{
#define MAX_HANDLE_CARDS 17
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
        type = static_cast<CardType>((input & 0xf0 >> 8));
        num = static_cast<CardNum>(input & 0x0f);
    }
    CardType type;
    CardNum num;
    uint8_t getValue() const{
        return (static_cast<int>(type) << 8) | (static_cast<int>(num));
    }
};

enum class Illegal{
    ROCKET                      = 0xff,        

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

    
    SINGLE                      = 0x1,

    PAIR                        = 0x2,

    TRIPLE_PURE                 = 0x3,
    TRIPLE_WITH_SINGLE          = 0x4,
    TRIPLE_WITH_PAIR            = 0x5,

    CHAIN_5                     = 0x6,
    CHAIN_6                     = 0x7,
    CHAIN_7                     = 0x8,
    CHAIN_8                     = 0x9,
    CHAIN_9                     = 0xa,
    CHAIN_10                    = 0xb,
    CHAIN_11                    = 0xc,
    CHAIN_12                    = 0xd,          //3 ~ ACE 12cards

    PAIR_CHAIN_3                = 0xe,
    PAIR_CHAIN_4                = 0xf,
    PAIR_CHAIN_5                = 0x10,
    PAIR_CHAIN_6                = 0x11,
    PAIR_CHAIN_7                = 0x12,
    PAIR_CHAIN_8                = 0x13,     // 16 cards

    FOUR_WITH_TWO_SINGLES        = 0x14,    //6
    FOUR_WITH_TWO_PAIRS          = 0x15,    //8

    //飞机的三条部分不能带2
    AIRPLANE_2_NOT_SWINGS        = 0x16,
    AIRPLANE_3_NOT_SWINGS        = 0x17,
    AIRPLANE_4_NOT_SWINGS        = 0x18,
    AIRPLANE_5_NOT_SWINGS        = 0x19,     //15 cards

    AIRPLANE_2_WITH_SMALL_SWINGS     = 0x1a, //4 cards
    AIRPLANE_3_WITH_SMALL_SWINGS     = 0x1b, //8 cards
    AIRPLANE_4_WITH_SMALL_SWINGS     = 0x1c, //12 cards
    AIRPLANE_5_WITH_SMALL_SWINGS     = 0x1d, //16 cards

    AIRPLANE_3_WITH_BIG_SWINGS       = 0x1e, // 5 cards
    AIRPLANE_4_WITH_BIG_SWINGS       = 0x1f, // 10 cards
    AIRPLANE_5_WITH_BIG_SWINGS       = 0x20, // 15 cards

    //航天飞机
#ifdef USE_SHUTTLE
    SPACE_SHUTTLE_2_NOT_SWINGS                   = 0x21,//3333 4444
    SPACE_SHUTTLE_3_NOT_SWINGS                   = 0x22,//
    SPACE_SHUTTLE_4_NOT_SWINGS                   = 0x23,//16 cards

    SPACE_SHUTTLE_2_WITH_SWINGS                   = 0x23,//3333 4444 5678
    SPACE_SHUTTLE_3_WITH_SWINGS                   = 0x23,//16 cards
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
    
    //get info
    // int cardNums[13] = {0,0,0,0,0,  0,0,0,0,0,  0,0,0};
    const Card* smallJoker = nullptr;
    const Card* bigJoker = nullptr;
    std::vector<const Card*> laiziVec; //癞子,癞子效果都一样，不用在区分如何组合癞子
    std::vector<const Card*> normalVec[13];

    for(const auto& one: parseCard)
    {
        if(one.type == CardType::LAIZI){
            laiziVec.emplace_back(&one);
        }else if(one.type == CardType::SMALL_JOKER){
            smallJoker = &one;
        }else if(one.type == CardType::BIG_JOKER){
            bigJoker = &one;
        }else{
            // ++cardNums[static_cast<int>(one.num) + 1];
            normalVec[static_cast<int>(one.num)].emplace_back(&one);
        }
    }

    //rocket
    if(smallJoker != nullptr && bigJoker != nullptr){
        std::vector<uint8_t> rocket{smallJoker->getValue(),bigJoker->getValue()};
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
    // 小于4 则不用计算，肯定不满足条件，
    if(lastMaxNum + laiziNum >= 4){
        //bomb 8 pure joker
        if(laiziNum >= 8){
            std::vector<uint8_t> bomb;
            for(int i = 0; i < 8; i++){
                bomb.emplace_back(laiziVec[i]->getValue());
            }
            result.emplace_back(Illegal::BOMB_8_PURE_JOKER,bomb);
        }

        //bomb 8 comb
        for(int i = 0; i < 13; i++){
            const auto& currentNumVec = normalVec[i];
            auto num = currentNumVec.size();
            if(num > 0 && num + laiziNum >= 8){
                std::vector<uint8_t> bomb;
                int j = 0;
                for (j = 0; j < num; j++){
                    bomb.emplace_back(currentNumVec[i]->getValue());
                }
                for(int i = 0; i < 8 - j; i++){
                    bomb.emplace_back(laiziVec[i]->getValue());
                }
                result.emplace_back(Illegal::BOMB_8_COMB,bomb);
            }
        }

        //bomb 7 pure joker
        if(laiziNum >= 7){
            //4 3 or 3 4
            std::vector<uint8_t> bomb;
            for(int i = 0; i < 7; i++){
                bomb.emplace_back(laiziVec[i]->getValue());
            }
            result.emplace_back(Illegal::BOMB_7_PURE_JOKER,bomb);
        }

        //bomb 7 comb
        for(int i = 0; i < 13; i++){
            const auto& currentNumVec = normalVec[i];
            auto num = currentNumVec.size();
            if(num > 0 && num + laiziNum >= 7){
                std::vector<uint8_t> bomb;
                int j = 0;
                for (j = 0; j < num; j++){
                    bomb.emplace_back(currentNumVec[i]->getValue());
                }
                for(int i = 0; i < 7 - j; i++){
                    bomb.emplace_back(laiziVec[i]->getValue());
                }
                result.emplace_back(Illegal::BOMB_7_COMB,bomb);
            }
        }

        //bomb 6 pure joker
        if(laiziNum >= 6){
            //4 3 or 3 4
            std::vector<uint8_t> bomb;
            for(int i = 0; i < 6; i++){
                bomb.emplace_back(laiziVec[i]->getValue());
            }
            result.emplace_back(Illegal::BOMB_6_PURE_JOKER,bomb);
        }

        //bomb 6 comb
        for(int i = 0; i < 13; i++){
            const auto& currentNumVec = normalVec[i];
            auto num = currentNumVec.size();
            if(num > 0 && num + laiziNum >= 6){
                std::vector<uint8_t> bomb;
                int j = 0;
                for (j = 0; j < num; j++){
                    bomb.emplace_back(currentNumVec[i]->getValue());
                }
                for(int i = 0; i < 6 - j; i++){
                    bomb.emplace_back(laiziVec[i]->getValue());
                }
                result.emplace_back(Illegal::BOMB_6_COMB,bomb);
            }
        }

        //bomb 5 pure joker
        if(laiziNum >= 5){
            //4 3 or 3 4
            std::vector<uint8_t> bomb;
            for(int i = 0; i < 5; i++){
                bomb.emplace_back(laiziVec[i]->getValue());
            }
            result.emplace_back(Illegal::BOMB_5_PURE_JOKER,bomb);
        }

        //bomb 5 comb
        for(int i = 0; i < 13; i++){
            const auto& currentNumVec = normalVec[i];
            auto num = currentNumVec.size();
            if(num > 0 && num + laiziNum >= 5){
                std::vector<uint8_t> bomb;
                int j = 0;
                for (j = 0; j < num; j++){
                    bomb.emplace_back(currentNumVec[i]->getValue());
                }
                for(int i = 0; i < 5 - j; i++){
                    bomb.emplace_back(laiziVec[i]->getValue());
                }
                result.emplace_back(Illegal::BOMB_5_COMB,bomb);
            }
        }

        //bomb 4 pure joker
        if(laiziNum >= 4){
            //4 3 or 3 4
            std::vector<uint8_t> bomb;
            for(int i = 0; i < 4; i++){
                bomb.emplace_back(laiziVec[i]->getValue());
            }
            result.emplace_back(Illegal::BOMB_4_PURE_JOKER,bomb);
        }

        //bomb 4 comb
        for(int i = 0; i < 13; i++){
            const auto& currentNumVec = normalVec[i];
            auto num = currentNumVec.size();
            //4 pure bomb already record
            if(num > 0 && num + laiziNum >= 4){
                std::vector<uint8_t> bomb;
                int j = 0;
                for (j = 0; j < num; j++){
                    bomb.emplace_back(currentNumVec[i]->getValue());
                }
                for(int i = 0; i < 4 - j; i++){
                    bomb.emplace_back(laiziVec[i]->getValue());
                }
                if(j == 4){
                    result.emplace_back(Illegal::BOMB_4_WITHOU_JOKER,bomb);
                }else{
                    result.emplace_back(Illegal::BOMB_4_COMB,bomb);
                }
            }
        }
    }


}


}



#endif//__DDZ_ALGORITHM_H__
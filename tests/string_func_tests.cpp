#include <cstring>
#include <gtest/gtest.h>

#include "../src/client_thread.h"
#include "../src/string_functions.h"

#include <vector>
#include <string>

TEST(StringTests, FindName){
    std::vector<std::string> messages
    {
        "imie Mikolaj message",
        "imie Lukasz   xxxxx",
        "imie robert"
    };
    std::vector<char> pointedChar = {'M', 'L', 'r'};
    char buf[100];
    for(int i = 0; i < messages.size(); i++){
        strcpy(buf, messages[i].c_str());
        char *nameFound = find_name(buf);
        ASSERT_NE(nameFound, nullptr);
        EXPECT_EQ(*nameFound, pointedChar[i]);
    }
}

TEST(StringTests, FindName_ReturnNullWhenNoName){
    std::vector<std::string> messages{
        "random message",
        "in the middle imie that should not be found",
        "at the end imie"
    };
    char buf[100];
    for(int i = 0; i < messages.size(); i++){
        strcpy(buf, messages[i].c_str());
        char *nameFound = find_name(buf);
        EXPECT_EQ(nameFound, nullptr);
    }
}

TEST(StringTests, BlockMessagesWithBannedWords){
    std::vector<std::string> messages{
        "blockme at the beggining",
        "in the blockme middle",
        "at the end blockme"
    };
    char buf[100];
    for(int i = 0; i < messages.size(); i++){
        strcpy(buf, messages[i].c_str());
        bool blocked = check_if_blocked(buf);
        EXPECT_EQ(blocked, true);
    }
}

TEST(StringTests, DontBlockIfAllGood){
    std::vector<std::string> messages{
        "regular message",
        "tricky block me",
        "blockm blockem blockmme block me"
    };
    char buf[100];
    for(int i = 0; i < messages.size(); i++){
        strcpy(buf, messages[i].c_str());
        bool blocked = check_if_blocked(buf);
        EXPECT_EQ(blocked, false);
    }
}

TEST(StringTests, GetNameLength){
    std::vector<std::string> messages{
        "imie Mikolaj message",
        "imie Lukasz   xxxxx",
        "imie robert"
    };
    int startingIndex = 5;
    std::vector<int> results{7, 6, 6};
    char buf[100];
    for(int i = 0 ; i < messages.size(); i++) {
        strcpy(buf, messages[i].c_str());
        int answer = get_name_length(buf + startingIndex);
        EXPECT_EQ(answer, results[i]);
    }
}

TEST(StringTests, ClearTelnetMessage){
    std::vector<std::string> messages{
        "msg1 with strange signs\r\b\n",
        "random message 2 with no strange signs",
        "message\awith\bmany\rstrange\vsigns\f\n\b"
    };

    std::vector<std::string> expectedResults{
        "msg1 with strange signs  \n",
        "random message 2 with no strange signs",
        "message with many strange signs \n "
    };

    char buf[100];
    char expect[100];
    for(int i = 0; i < messages.size(); i++){
        strcpy(buf, messages[i].c_str());
        clear_telnet_msg(buf, messages[i].size());
        strcpy(expect, expectedResults[i].c_str());
        EXPECT_STREQ(buf, expect);
    }
}

TEST(StringTests, SetClientName){

    Client client;
    std::vector<std::string> messages{
        "imie Maciej rest of the message",
        "imie John rest of the message",
        "imie Andrew"
    };

    std::vector<std::string> expectedNames{
        "Maciej",
        "John",
        "Andrew"
    };

    char buf[100];
    char expected[100];
    for(int i = 0; i < messages.size(); i++){
        strcpy(buf, messages[i].c_str());
        set_client_name(&client, buf);
        strcpy(expected, expectedNames[i].c_str());
        EXPECT_STREQ(client.name, expected);
    }
}

TEST(StringTests, PrepareMessageForClient){
    std::vector<std::string> messages{
        "example message 1",
        "some specific message",
        "random letters mfaefuinaefe"
    };

    std::vector<std::string> names{
        "imie Matthew",
        "imie George",
        "imie Donald"
    };

    std::vector<std::string> expectedMessages{
        "Matthew: example message 1",
        "George: some specific message",
        "Donald: random letters mfaefuinaefe"
    };

    char buf1[100];
    char buf2[100];
    char expected[100];
    Client client;
    for(int i = 0; i < messages.size(); i++){
        strcpy(buf1, names[i].c_str());
        set_client_name(&client, buf1);
        strcpy(buf2, messages[i].c_str());
        prepare_messege_for_client(&client, buf2);
        strcpy(expected, expectedMessages[i].c_str());
        EXPECT_STREQ(client.last_msg, expected);
    }
}
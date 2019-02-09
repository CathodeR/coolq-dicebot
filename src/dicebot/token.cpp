#include "./token.h"

#include <list>

using namespace dicebot;
using namespace diceparser;

token_streamer::token_streamer(const std::string & source){
    this->src.assign(source);
    this->pos_cur = this->pos_next = 0;
    using bracket_pair = std::pair<char,size_t>;
    std::list<bracket_pair> chaser;
    bool terminate = false;
    int good_for_comma = 0;
    size_t i = 0;
    for(; i < src.size() ;i++){
        char lbrack = ')';
        const char & c_here = src[i];
        switch (c_here)
        {
            case '{':
                good_for_comma++;
            case '(':
                chaser.push_back(bracket_pair(c_here,i));
                break;
            case '}':
                lbrack = '{';
                good_for_comma--;
            case ')':
                if(chaser.size() == 0){
                    terminate = true;
                    break;
                }
                if(chaser.back().first == lbrack){
                    chaser.pop_back();
                }
                else terminate = true;
                break;
            case ',':
                if(good_for_comma == 0){
                    terminate = true;
                    break;
                }
            default:
                break;
        }
        if(terminate) break;
    }
    if(chaser.size() > 0)
        this->src_max = chaser.front().second;
    else
        this->src_max = i;
}

bool token_streamer::next_punctuator(){
    for(const punctuator_container & punc : punctuator_arr){
        if(punc.name == this->src[pos_cur]){
            this->ret_token.id = token_index::index_punctuator;
            this->ret_token.pc = punc.pc;
            this->pos_next = this->pos_cur + 1;
            return true;
        }
    }
    return false;
}

bool token_streamer::next_keyword(){
    size_t max_len = this->src_max - this->pos_cur;
    for(const keyword_container & keyw : keyword_arr){
        size_t i = 0;
        for(; keyw.name[i] && i < max_len; i++){
            if(keyw.name[i] == this->src[this->pos_cur + i]) continue;
            else break;
        }
        if(keyw.name[i]) continue;
        else{
            this->ret_token.id = token_index::index_keyword;
            this->ret_token.kw = keyw.kw;
            this->pos_next = this->pos_cur + i;
            return true;
        }
    }
    return false;
}

bool token_streamer::next_number(){
    size_t max_len = this->src_max - this->pos_cur;
    bool dotted = false;

    size_t start = this->pos_cur;
    if(this->src[start] == '-') start += 1;
    if(this->src[start] == '.') {dotted = true; start += 1;}

    size_t i = start;
    for(; i < this->src_max; i++){
        char a = this->src[i];
        if(a <= '9' && a >= '0') continue;
        else if(!dotted && a == '.') {dotted = true; continue;}
        else if(a == '%') {i++; break;}
        else break;
    }
    if(i>start){
        this->ret_token.id = token_index::index_number;
        this->pos_next = i;
        return true;
    }
    return false;
}

token_t * token_streamer::cur_token(){
    return &(this->ret_token);
}

token_t * token_streamer::next_token(){
    this->pos_cur = this->pos_next;
    while(this->src[this->pos_cur] == ' ') this->pos_cur++;
    if(this->pos_cur == this->src_max){
        this->ret_token.id = token_index::index_stop;
        return &(this->ret_token);
    }
    bool is_got = this->next_number() || this->next_punctuator() || this->next_keyword();
    if(!is_got) this->ret_token.id = token_index::index_stop;
    return &(this->ret_token);
}

std::string token_streamer::cur_token_string(){
    return this->src.substr(this->pos_cur,this->pos_next - this->pos_cur);
}
#pragma once

#include "../common.h"

#include "../../cqsdk/utils/vendor/cpp-base64/base64.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace dicebot::profile{
    
    class var_pair:public std::pair<int,int>{
    public:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & this->first;
            ar & this->second;
        }

        var_pair(int first,int second) : std::pair<int,int>(first,second){}
        var_pair(){};
    };
    
    #pragma region profile template

    template <class _profile_Key, class _profile_Val>
    class profile_template: public std::map<_profile_Key,_profile_Val>{
        using profile_map = typename std::map<_profile_Key,_profile_Val>;
        using profile_pair = typename std::pair<_profile_Key,_profile_Val>;
        using profile_citer = typename profile_map::const_iterator;
        using profile_iter = typename profile_map::iterator;
    public:
        profile_status get(_profile_Key const & var_type, _profile_Val & var) const noexcept{
            profile_citer target = this->find(var_type);
            if(target != this->cend()){
                var = target->second;
                return profile_status::finished;
            }
            else return profile_status::not_exist;
        }

        profile_status set(_profile_Key const & var_type, _profile_Val const & new_var) noexcept{
            profile_iter target = this->find(var_type);
            if(target != this->end()){
                target->second = new_var;
                return profile_status::finished;
            }else{
                auto t = this->insert(profile_pair(var_type,new_var));
                if(t.second) return profile_status::finished;
                else return profile_status::not_exist; 
            }
        }

        profile_status add(_profile_Key const & var_type, _profile_Val const & new_var) noexcept{
            profile_iter target = this->find(var_type);
            if(target == this->end()){
                this->insert(std::pair<_profile_Key,_profile_Val>(var_type,new_var));
                return profile_status::finished;
            }
            else return profile_status::exceed;
        }
        
        std::string encode() const noexcept{
            ostrs strs(ostrs::ate);
            boost::archive::binary_oarchive oa(strs);
            oa << this->size();
            auto iter_list = this->cbegin();
            for (; iter_list != this->cend(); iter_list++){
                oa << ((*iter_list).first);
                oa << ((*iter_list).second);
            }
            return base64_encode((const unsigned char *)(strs.str().c_str()),strs.str().size());
        }

        bool decode(std::string const & source) noexcept{
            this->clear();
            //this->swap(std::map<_profile_Key,_profile_Val>());
            std::string source_copy(source);
            source_copy = base64_decode(source_copy);
            std::istringstream iss(source_copy);
            boost::archive::binary_iarchive ia(iss);
            size_t len = 0;
            ia >> len;
            try{
                for (uint32_t i_iter = 0; i_iter < len; i_iter++){
                    _profile_Key first;
                    ia >> first;
                    _profile_Val second;
                    ia >> second;
                    this->insert(profile_pair(first,second));
                }
            }
            catch(std::exception e){
                #ifdef _DEBUG
                logger::log("dicebot profile",std::string(e.what()));
                #endif
                return false;
            }
            return true;
        }
    };
    #pragma endregion

    #pragma region user profile
    class user_profile{
    public:
        using sys_var_map_t = profile_template<sys_var_type,var_pair>;
        using user_var_map_t = profile_template<std::string,var_pair>;
        using def_roll_map_t = profile_template<def_roll_type,std::string>;
        using mac_roll_map_t = profile_template<std::string,std::string>;
    private:
        sys_var_map_t sys_vars;
        user_var_map_t user_vars;
        def_roll_map_t def_roll;
        mac_roll_map_t mac_rolls;
        static bool init_sys_var(sys_var_type const type,var_pair & val){
            switch(type){
            case sys_var_type::rs_on:
                val.first  = val.second = 1;
                return true;
            default:
                return false;
            }
        }
    public:

        bool reset_var(){
            auto iter = user_vars.begin();
            for(;iter != user_vars.end();iter++){
                iter->second.second = iter->second.first;
            }
            return true;
        }

        template <class _profile_Key, class _profile_Val>
        bool get_var(_profile_Key const & key, _profile_Val & val){
            return false;
        }

        template <class _profile_Key, class _profile_Val>
        bool set_var(_profile_Key const & key, _profile_Val const & val){
            return false;
        }
        
        template <class _profile_Key, class _profile_Val>
        profile_template<_profile_Key,_profile_Val> * get_map(){
            return nullptr;
        }

        bool encode(std::vector<std::string> & target) const{
            target.clear();
            target.push_back(this->sys_vars.encode());
            target.push_back(this->user_vars.encode());
            target.push_back(this->def_roll.encode());
            target.push_back(this->mac_rolls.encode());
            return true;
        }

        bool decode(std::vector<std::string> & target){
            if(target.size() != 4) return false;
            if(!this->sys_vars.decode(target[0])) return false;
            if(!this->user_vars.decode(target[1])) return false;
            if(!this->def_roll.decode(target[2])) return false;
            if(!this->mac_rolls.decode(target[3])) return false;
            return true;
        }
    };

    template <>
    bool inline user_profile::get_var<user_profile::sys_var_map_t::key_type, user_profile::sys_var_map_t::mapped_type>(
        user_profile::sys_var_map_t::key_type const & key, 
        user_profile::sys_var_map_t::mapped_type & val){
        if(sys_vars.get(key,val) == profile_status::finished) return true;
        else{
            init_sys_var(key,val);
            return sys_vars.add(key,val) == profile_status::finished;
        }
    }

    template <>
    bool inline user_profile::set_var<user_profile::sys_var_map_t::key_type, user_profile::sys_var_map_t::mapped_type>(
        user_profile::sys_var_map_t::key_type const & key, 
        user_profile::sys_var_map_t::mapped_type const & val){
        if(sys_vars.set(key,val) == profile_status::finished) return true;
        else{
            return sys_vars.add(key,val) == profile_status::finished;
        }
    }

    #define easy_get_var(_type,_target) template <>\
    inline bool user_profile::get_var<_type::key_type,_type::mapped_type>(_type::key_type const & key, _type::mapped_type & val){\
        return _target.get(key,val) == profile_status::finished;}

    #define easy_set_var(_type,_target) template <>\
    inline bool user_profile::set_var<_type::key_type,_type::mapped_type>(_type::key_type const & key, _type::mapped_type const & val){\
        return _target.set(key,val) == profile_status::finished;}

    easy_get_var(user_profile::user_var_map_t, user_vars);
    easy_set_var(user_profile::user_var_map_t, user_vars);
    easy_get_var(user_profile::def_roll_map_t, def_roll);
    easy_set_var(user_profile::def_roll_map_t, def_roll);
    easy_get_var(user_profile::mac_roll_map_t, mac_rolls);
    easy_set_var(user_profile::mac_roll_map_t, mac_rolls);

    #define easy_get_map(_type,_target) template <>\
    inline _type * user_profile::get_map<_type::key_type,_type::mapped_type>(){return &_target;}

    easy_get_map(user_profile::user_var_map_t, user_vars);
    easy_get_map(user_profile::mac_roll_map_t, mac_rolls);
    easy_get_map(user_profile::def_roll_map_t, def_roll);

    #pragma endregion
}
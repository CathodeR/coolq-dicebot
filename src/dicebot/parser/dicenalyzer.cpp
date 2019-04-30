#include "dicenalyzer.h"
#include <functional>
#include <sstream>
#include "../dice_roller.h"
using namespace dicebot;
using namespace diceparser;

static void recurse_dicelet_ct(syntax_item *node, std::vector<p_component> &container) {
    if (!node->is_terminal()) {
        syntax_nterminal *nt_node = dynamic_cast<syntax_nterminal *>(node);
        if (nt_node->prod_type == production_type::dicelet_ct_const_expr || nt_node->prod_type == production_type::dicelet_ct_rand_expr) {
            container.push_back(build_component_from_syntax(nt_node->items[0].get()));
        } else if (nt_node->prod_type == production_type::dicelet_ct_dicelet_ct_comma_const_expr
                   || nt_node->prod_type == production_type::dicelet_ct_dicelet_ct_comma_rand_expr) {
            recurse_dicelet_ct(nt_node->items[0].get(), container);
            container.push_back(build_component_from_syntax(nt_node->items[2].get()));
        }
    }
}

static inline char operator_from_syntax_item(const syntax_item *from) {
    const syntax_terminal *temp = dynamic_cast<const syntax_terminal *>(from);
    return temp->source[0];
}

static inline dicelet::p_dicelet dicelet_from_normal_component(const syntax_item *root) {
    auto ret = std::make_shared<dicelet_unit>();
    ret->dicelets.resize(1, build_component_from_syntax(root));
    return std::dynamic_pointer_cast<dicelet>(ret);
}

static auto const_calcucation = [](const syntax_nterminal *p_syntax_item) -> auto {
    auto ret = std::make_shared<comp_number>();
    auto p1 = build_component_from_syntax(p_syntax_item->items[0].get());
    auto p2 = build_component_from_syntax(p_syntax_item->items[2].get());
    char oper = operator_from_syntax_item(p_syntax_item->items[1].get());
    auto what1 = std::dynamic_pointer_cast<comp_number>(p1)->what;
    auto what2 = std::dynamic_pointer_cast<comp_number>(p2)->what;
    switch (oper) {
    case '+':
        ret->what = what1 + what2;
        break;
    case '-':
        ret->what = what1 - what2;
        break;
    case '*':
        ret->what = what1 * what2;
        break;
    case '/':
        ret->what = what1 / what2;
        break;
    }
    return ret;
};

static auto build_rdk = [](dice_rdk_mode mode, const syntax_nterminal *p_syntax_item) -> auto {
    auto ret = std::make_shared<comp_dice_rdk>();
    ret->mode = mode;
    switch (mode) {
    case dice_rdk_mode::single_d: {
        p_component p1 = build_component_from_syntax(p_syntax_item->items[1].get());
        ret->face = std::dynamic_pointer_cast<comp_number>(p1)->what.force_positive_int();
        break;
    }
    case dice_rdk_mode::numbered_d: {
        p_component p1 = build_component_from_syntax(p_syntax_item->items[0].get());
        p_component p2 = build_component_from_syntax(p_syntax_item->items[2].get());
        ret->dice = std::dynamic_pointer_cast<comp_number>(p1)->what.force_positive_int();
        ret->face = std::dynamic_pointer_cast<comp_number>(p2)->what.force_positive_int();
        break;
    }
    case dice_rdk_mode::numbered_d_k:
    case dice_rdk_mode::numbered_d_kl: {
        p_component p1 = build_component_from_syntax(p_syntax_item->items[0].get());
        p_component p2 = build_component_from_syntax(p_syntax_item->items[2].get());
        ret->dice = std::dynamic_pointer_cast<comp_number>(p1)->what.force_positive_int();
        ret->face = std::dynamic_pointer_cast<comp_number>(p2)->what.force_positive_int();
        p_component p3 = build_component_from_syntax(p_syntax_item->items[4].get());
        ret->keep = std::dynamic_pointer_cast<comp_number>(p3)->what.force_positive_int();
        break;
    }
    default:
        return decltype(ret)();
    }
    return ret;
};

p_component diceparser::build_component_from_syntax(const syntax_item *root) {
    if (root->is_terminal()) {
        if (root->type == terminal_number) {
            const syntax_terminal *p_syntax_item = dynamic_cast<const syntax_terminal *>(root);
            auto ret = std::make_shared<comp_number>();
            ret->what = p_syntax_item->source;
            return ret;
        } else
            return nullptr;
    } else {
        const syntax_nterminal *p_syntax_item = dynamic_cast<const syntax_nterminal *>(root);
        switch (p_syntax_item->prod_type) {
        case production_type::acc_rand_expr:
        case production_type::acc_const_expr:
        case production_type::acc_dicelet_expr:
        case production_type::const_unit_number:
        case production_type::const_mul_const_unit:
        case production_type::const_expr_const_mul:
        case production_type::rand_mul_rand_unit:
        case production_type::rand_expr_rand_mul:
        case production_type::dicelet_expr_dicelet_mul:
        case production_type::dicelet_mul_dicelet_u: {
            return build_component_from_syntax(p_syntax_item->items[0].get());
        }
        case production_type::const_unit_lparenthesis_const_expr_rparenthesis: {
            return build_component_from_syntax(p_syntax_item->items[1].get());
        }
        case production_type::rand_unit_lparenthesis_rand_expr_rparenthesis: {
            auto ret = std::make_shared<comp_holder>();
            ret->child = build_component_from_syntax(p_syntax_item->items[1].get());
            return ret;
        }
        case production_type::const_expr_minus_const_mul: {
            auto p1 = build_component_from_syntax(p_syntax_item->items[1].get());
            std::dynamic_pointer_cast<comp_number>(p1)->what = dicebot::zero - std::dynamic_pointer_cast<comp_number>(p1)->what;
            return p1;
        }
        case production_type::const_mul_const_mul_muldvi_const_unit:
        case production_type::const_expr_const_expr_minus_const_mul:
        case production_type::const_expr_const_expr_plus_const_mul: {
            return const_calcucation(p_syntax_item);
        }
        case production_type::rand_unit_d_const_unit: {
            return build_rdk(dice_rdk_mode::single_d, p_syntax_item);
        }
        case production_type::rand_unit_const_unit_d_const_unit: {
            return build_rdk(dice_rdk_mode::numbered_d, p_syntax_item);
        }
        case production_type::rand_unit_const_unit_d_const_unit_k_const_unit: {
            return build_rdk(dice_rdk_mode::numbered_d_k, p_syntax_item);
        }
        case production_type::rand_unit_const_unit_d_const_unit_kl_const_unit: {
            return build_rdk(dice_rdk_mode::numbered_d_kl, p_syntax_item);
        }
        case production_type::rand_expr_minus_rand_mul: {
            auto ret = std::make_shared<comp_calculus_reverse>();
            ret->child = build_component_from_syntax(p_syntax_item->items[1].get());
            return ret;
        }
        case production_type::rand_mul_const_mul_muldvi_rand_unit:
        case production_type::rand_mul_rand_mul_muldvi_const_unit:
        case production_type::rand_mul_rand_mul_muldvi_rand_unit:
        case production_type::rand_expr_const_expr_plus_rand_mul:
        case production_type::rand_expr_const_expr_minus_rand_mul:
        case production_type::rand_expr_rand_expr_plus_const_mul:
        case production_type::rand_expr_rand_expr_minus_const_mul:
        case production_type::rand_expr_rand_expr_plus_rand_mul:
        case production_type::rand_expr_rand_expr_minus_rand_mul: {
            auto ret = std::make_shared<comp_calculus>();
            ret->lchild = build_component_from_syntax(p_syntax_item->items[0].get());
            ret->what = operator_from_syntax_item(p_syntax_item->items[1].get());
            ret->rchild = build_component_from_syntax(p_syntax_item->items[2].get());
            return ret;
        }
        case production_type::dicelet_u_const_unit_sharp_const_unit:
        case production_type::dicelet_u_const_unit_sharp_rand_unit: {
            auto ret = std::make_shared<dicelet_unit>();
            p_component p1 = build_component_from_syntax(p_syntax_item->items[0].get());
            uint16_t count = std::dynamic_pointer_cast<comp_number>(p1)->what.force_positive_int();
            ret->dicelets.resize(count, build_component_from_syntax(p_syntax_item->items[2].get()));
            return ret;
        }
        case production_type::dicelet_u_lbrace_dicelet_ct_rbrace: {
            auto ret = std::make_shared<dicelet_unit>();
            recurse_dicelet_ct(p_syntax_item->items[1].get(), ret->dicelets);
            return ret;
        }
        case production_type::dicelet_mul_const_mul_muldvi_dicelet_u:
        case production_type::dicelet_mul_rand_mul_muldvi_dicelet_u:
        case production_type::dicelet_expr_const_expr_minus_dicelet_mul:
        case production_type::dicelet_expr_const_expr_plus_dicelet_mul:
        case production_type::dicelet_expr_rand_expr_minus_dicelet_mul:
        case production_type::dicelet_expr_rand_expr_plus_dicelet_mul: {
            auto ret = std::make_shared<dicelet_calculus>();
            ret->lchild = dicelet_from_normal_component(p_syntax_item->items[0].get());
            ret->what = operator_from_syntax_item(p_syntax_item->items[1].get());
            ret->rchild = std::dynamic_pointer_cast<dicelet>(build_component_from_syntax(p_syntax_item->items[2].get()));
            return ret;
        }
        case production_type::dicelet_mul_dicelet_mul_muldvi_const_unit:
        case production_type::dicelet_mul_dicelet_mul_muldvi_rand_unit:
        case production_type::dicelet_expr_dicelet_expr_minus_const_mul:
        case production_type::dicelet_expr_dicelet_expr_minus_rand_mul:
        case production_type::dicelet_expr_dicelet_expr_plus_const_mul:
        case production_type::dicelet_expr_dicelet_expr_plus_rand_mul: {
            auto ret = std::make_shared<dicelet_calculus>();
            ret->lchild = std::dynamic_pointer_cast<dicelet>(build_component_from_syntax(p_syntax_item->items[0].get()));
            ret->what = operator_from_syntax_item(p_syntax_item->items[1].get());
            ret->rchild = dicelet_from_normal_component(p_syntax_item->items[2].get());
            return ret;
        }
        case production_type::dicelet_mul_dicelet_mul_muldvi_dicelet_u:
        case production_type::dicelet_expr_dicelet_expr_minus_dicelet_mul:
        case production_type::dicelet_expr_dicelet_expr_plus_dicelet_mul: {
            auto ret = std::make_shared<dicelet_calculus>();
            ret->lchild = std::dynamic_pointer_cast<dicelet>(build_component_from_syntax(p_syntax_item->items[0].get()));
            ret->what = operator_from_syntax_item(p_syntax_item->items[1].get());
            ret->rchild = std::dynamic_pointer_cast<dicelet>(build_component_from_syntax(p_syntax_item->items[2].get()));
            return ret;
        }
        case production_type::dicelet_expr_minus_dicelet_mul: {
            auto ret = std::make_shared<dicelet_calculus_reverse>();
            ret->child = std::dynamic_pointer_cast<dicelet>(build_component_from_syntax(p_syntax_item->items[2].get()));
            return ret;
        }
        default:
            return nullptr;
        }
    }
}

number comp_number::roll_the_dice(str_container &out) const {
    out.emplace_back(this->what.str());
    return this->what;
}
void comp_number::print(str_container &strlist) const noexcept { strlist.push_back(this->what.str()); }

number comp_holder::roll_the_dice(str_container &out) const {
    out.emplace_back("(");
    number ret = this->child->roll_the_dice(out);
    out.emplace_back(")");
    return ret;
}
void comp_holder::print(str_container &strlist) const noexcept {
    strlist.emplace_back("(");
    this->child->print(strlist);
    strlist.emplace_back(")");
}

number comp_dice_rdk::roll_the_dice(str_container &out) const {
    dicebot::roll::dice_roll dr;
    switch (this->mode) {
    case dice_rdk_mode::single_d: {
        dicebot::roll::roll_rdk(dr, 1, this->face, 1);
        break;
    }
    case dice_rdk_mode::numbered_d: {
        dicebot::roll::roll_rdk(dr, this->dice, this->face, this->dice);
        break;
    }
    case dice_rdk_mode::numbered_d_k: {
        dicebot::roll::roll_rdk(dr, this->dice, this->face, this->keep);
        break;
    }
    case dice_rdk_mode::numbered_d_kl: {
        dicebot::roll::roll_rdk(dr, this->dice, this->face, -static_cast<signed>(this->keep));
        break;
    }
    default:
        break;
    }

    switch (dr.status) {
    case roll::roll_status::FINISHED: {
        out.emplace_back(dr.detail());
        return dr.summary;
    }
    case roll::roll_status::TOO_MANY_DICE: {
    }
    case roll::roll_status::DICE_NOT_AVAILABLE: {
    }
    default:
        return 0;
    }
}
void comp_dice_rdk::print(str_container &strlist) const noexcept {
    std::ostringstream strss;
    switch (this->mode) {
    case dice_rdk_mode::single_d:
        strss << "d" << this->face;
        break;
    case dice_rdk_mode::numbered_d:
        strss << this->dice << "d" << this->face;
        break;
    case dice_rdk_mode::numbered_d_k:
        strss << this->dice << "d" << this->face << "k" << this->keep;
        break;
    case dice_rdk_mode::numbered_d_kl:
        strss << this->dice << "d" << this->face << "kl" << this->keep;
        break;
    }
    strlist.emplace_back(strss.str());
}

number comp_calculus::roll_the_dice(str_container &out) const {
    str_container temp_list;
    number left = this->lchild->roll_the_dice(temp_list);
    temp_list.emplace_back(std::string(1, this->what));
    number right = this->rchild->roll_the_dice(temp_list);
    out.splice(out.end(), temp_list);
    switch (this->what) {
    case '+':
        return left + right;
    case '-':
        return left - right;
    case '*':
        return left * right;
    case '/':
        return left / right;
    default:
        return number(0);
    }
}
void comp_calculus::print(str_container &strlist) const noexcept {
    this->lchild->print(strlist);
    std::string str = " ";
    str[0] = this->what;
    strlist.push_back(str);
    this->rchild->print(strlist);
}

number comp_calculus_reverse::roll_the_dice(str_container &out) const {
    out.emplace_back("-");
    return this->child->roll_the_dice(out);
}
void comp_calculus_reverse::print(str_container &strlist) const noexcept {
    strlist.emplace_back("-");
    this->child->print(strlist);
}

void dicelet_unit::roll_dicelet(result_container &rets, str_container &out) const {
    str_container str_temp;
    bool is_first = true;
    str_temp.emplace_back("{");
    for (const p_component &comp : this->dicelets) {
        if (is_first)
            is_first = false;
        else
            str_temp.emplace_back(",");
        rets.push_back(comp->roll_the_dice(str_temp));
    }
    str_temp.emplace_back("}");
    out.splice(out.end(), str_temp);
}
void dicelet_unit::print(str_container &strlist) const noexcept {
    strlist.emplace_back("{");
    bool is_first = true;
    for (p_component const &dlp : this->dicelets) {
        if (is_first)
            is_first = false;
        else
            strlist.emplace_back(", ");
        dlp->print(strlist);
    }
    strlist.emplace_back("}");
}

void dicelet_holder::roll_dicelet(result_container &rets, str_container &out) const {
    str_container str_temp;
    result_container ret_temp;
    str_temp.push_back("(");
    this->child->roll_dicelet(ret_temp, str_temp);
    str_temp.push_back(")");
    rets.splice(rets.end(), ret_temp);
    out.splice(out.end(), str_temp);
}
void dicelet_holder::print(str_container &strlist) const noexcept {
    strlist.emplace_back("(");
    this->child->print(strlist);
    strlist.emplace_back(")");
}

void dicelet_calculus::roll_dicelet(result_container &rets, str_container &out) const {
    result_container ret_temp_l;
    result_container ret_temp_r;
    str_container str_temp;
    this->lchild->roll_dicelet(ret_temp_l, str_temp);
    str_temp.emplace_back(std::string(1, this->what));
    this->rchild->roll_dicelet(ret_temp_r, str_temp);
    std::size_t n = std::max(ret_temp_l.size(), ret_temp_r.size());

    auto iter_l = ret_temp_l.begin();
    auto iter_r = ret_temp_r.begin();

    result_container ret_temp;

    while (n--) {
        switch (this->what) {
        case '+':
            ret_temp.emplace_back(*iter_l + *iter_r);
            break;
        case '-':
            ret_temp.emplace_back(*iter_l - *iter_r);
            break;
        case '*':
            ret_temp.emplace_back(*iter_l * *iter_r);
            break;
        case '/':
            ret_temp.emplace_back(*iter_l / *iter_r);
            break;
        default:
            break;
        }
        iter_l++;
        iter_r++;
        if (iter_l == ret_temp_l.end()) iter_l = ret_temp_l.begin();
        if (iter_r == ret_temp_l.end()) iter_r = ret_temp_r.begin();
    }
    rets.splice(rets.end(), ret_temp);
    out.splice(out.end(), str_temp);
}
void dicelet_calculus::print(str_container &strlist) const noexcept {
    this->lchild->print(strlist);
    strlist.emplace_back(std::string(1, this->what));
    this->rchild->print(strlist);
}

void dicelet_calculus_reverse::roll_dicelet(result_container &rets, str_container &out) const {
    result_container ret_temp;
    str_container str_temp;
    this->child->roll_dicelet(ret_temp, str_temp);
    for (number &num : ret_temp) {
        num = number(0) - num;
    }
    rets.splice(rets.end(), ret_temp);
    out.splice(out.end(), str_temp);
}
void dicelet_calculus_reverse::print(str_container &strlist) const noexcept {
    strlist.emplace_back("-");
    this->child->print(strlist);
}
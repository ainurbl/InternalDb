#include <string>
#include <utility>
#include <vector>
#include <map>

const std::map<int, std::string> OrdinalToType{
        {0, "INT"},
        {1, "REAL"},
        {2, "TEXT"},
        {3, "CHAR"},
        {4, "VARCHAR"},
        {5, "BOOLEAN"},
        {6, "UNKNOWN"},
};

const std::map<std::string, int> StringToOrdinal{
        {"INT",     0},
        {"REAL",    1},
        {"TEXT",    2},
        {"CHAR",    3},
        {"VARCHAR", 4},
        {"BOOLEAN", 5},
        {"UNKNOWN", 6}
};

constexpr static bool hasInternalInfo(int ordinal) {
    return ordinal == 4 || ordinal == 3;
}

struct TypeInternalInfo {
    int size;

    explicit TypeInternalInfo(int sz = 1) : size(sz) {}

    void addInternalInfo(std::string &res) const {
        res += "(" + std::to_string(size) + ")";
    }
};

struct Type {
    TypeInternalInfo info;
    int ordinal;

    explicit Type(const std::string &type) {
        auto ht = type.find('#');
        if (ht != std::string::npos) {
            const std::string _type = type.substr(0, ht);
            const std::string _info = type.substr(ht + 1);
            ordinal = StringToOrdinal.at(_type);
            info = TypeInternalInfo(std::stoi(_info));
        } else {
            ordinal = StringToOrdinal.at(type);
            info = TypeInternalInfo(1);
        }
    }

    [[nodiscard]] std::string toString() const {
        std::string result = OrdinalToType.at(ordinal);
        if (hasInternalInfo(ordinal)) {
            info.addInternalInfo(result);
        }
        return result;
    }
};

struct Value {
    Type type;
    std::string sval = "";
    float fval = 0.0f;
    int ival = 0;
    bool bval = false;

    explicit Value(std::string val): type("TEXT"), sval(std::move(val)) {}
    explicit Value(float val): type("REAL"), fval(val) {}
    explicit Value(int val): type("INT"), ival(val) {}
    explicit Value(bool val): type("BOOLEAN"), bval(val) {}

    [[nodiscard]] std::string toString() const {
        if (type.ordinal == StringToOrdinal.at("INT")) {
            return std::to_string(ival);
        }
        if (type.ordinal == StringToOrdinal.at("REAL")) {
            return std::to_string(fval);
        }
        if (type.ordinal == StringToOrdinal.at("TEXT")) {
            return sval;
        }
        if (type.ordinal == StringToOrdinal.at("BOOLEAN")) {
            return bval? "TRUE" : "FALSE";
        }
        throw(1);
    }
};

using NameToType = std::pair<std::string, Type>;
using ValueToType = std::pair<Value, Type>;

struct ExprEntity {
    Value *value;
    std::string *fieldName;

    [[nodiscard]] std::string toString() const {
        if (value != nullptr) {
            return value->toString();
        }
        if (fieldName != nullptr) {
            return *fieldName;
        }
        throw(1);
    }
};

namespace OperationEnum {
    enum Operation {
        NONE,
        EQUALS,
        NOT_EQUALS,
        KW_OR,
        KW_AND,
        KW_NOT,
        PLUS,
        MINUS,
        MULT,
        DIV,
    };

    static std::string toString(Operation operation) {
        switch (operation) {
            case NONE:
                return "NONE";
            case EQUALS:
                return "==";
            case NOT_EQUALS:
                return "!=";
            case KW_OR:
                return "OR";
            case KW_AND:
                return "AND";
            case KW_NOT:
                return "NOT";
            case PLUS:
                return "+";
            case MINUS:
                return "-";
            case MULT:
                return "*";
            case DIV:
                return "/";
        }
        return "";
    }
}

struct Expr {
    ExprEntity *entity;
    OperationEnum::Operation operation;
    Expr *l, *r;

    [[nodiscard]] std::string toString(int offset = 1) const {
        if (entity != nullptr) {
            return std::string(offset, ' ') + entity->toString();
        }
        auto tmp = std::string(offset, ' ') + OperationEnum::toString(operation);
        if (l != nullptr) {
            tmp += '\n' + l->toString(offset + 2);
        }
        if (r != nullptr) {
            tmp += '\n' + r->toString(offset + 2);
        }
        return tmp;
    }
};
#pragma once

#include <string>
#include <utility>
#include <vector>
#include <set>

#include "common.cc"

struct Query {
//    temporary fields
    std::string sval;
    int ival;
    float fval;
    Expr *expr;

//    query information
    std::string tableName;
    std::string operation;

//    CREATE
    std::vector<NameToType> createNameToType;
//    INSERT
    std::vector<ValueToType> insertValueToType;
//    SELECT
    std::vector<Expr*> selectExprs;
    Expr* selectLogExpr;
//    UPDATE
    std::map<std::string, Expr*> updateFieldToExpr;
    Expr* updateLogExpr;
//    DELETE
    Expr* deleteLogExpr;
};
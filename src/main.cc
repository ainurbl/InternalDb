#include <cstdio>
#include <map>
#include <functional>
#include <cassert>
#include <fstream>
#include <iostream>

#include "lexer.h"
#include "parser.h"


void printQueryReport(const Query &query) {
    printf("Operation: %s\nTableName: %s\n",
           query.operation.c_str(),
           query.tableName.c_str()
    );
    const static std::map<std::string, std::function<void()>> printer{
            {"CREATE", [&]() -> void {
                printf("Fields:\n");
                for (auto &[name, type]: query.createNameToType) {
                    printf("%s: %s\n", name.c_str(), type.toString().c_str());
                }
            }
            },
            {"INSERT", [&]() -> void {
                printf("Insertions:\n");
                for (auto &[value, type]: query.insertValueToType) {
                    printf("%s: %s\n", value.toString().c_str(), type.toString().c_str());
                }
            }
            },
            {"SELECT", [&]() -> void {
                printf("Selections:\n");
                if (query.selectExprs.empty()) {
                    printf(" *\n");
                } else {
                    for (auto &expr: query.selectExprs) {
                        printf("%s\n", expr->toString().c_str());
                    }
                }
                printf("Where:\n%s\n", query.selectLogExpr->toString().c_str());
            }
            },
            {"UPDATE", [&]() -> void {
                printf("Updates:\n");
                for (auto &[columnName, expr]: query.updateFieldToExpr) {
                    printf("%s:\n%s\n", columnName.c_str(), expr->toString().c_str());
                }
                printf("Where:\n%s\n", query.updateLogExpr->toString().c_str());
            }
            },
            {"DELETE", [&]() -> void {
                if (query.deleteLogExpr != nullptr) {
                    printf("Where:\n%s\n", query.deleteLogExpr->toString().c_str());
                }
            }
            },

    };
    printer.at(query.operation)();
}

void mainInteractive() {
    while (true) {
        printf("sql>");
        fflush(stdout);
        fflush(stdin);
        char input[1024];
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        if (input[0] == '\n') {
            continue;
        }
        YY_BUFFER_STATE state;
        if (!(state = yy_scan_bytes(input, strcspn(input, "\n")))) {
            continue;
        }
        Query ret;
        if (yyparse(&ret) == 0) {
            printQueryReport(ret);
        }
        yy_delete_buffer(state);
    }
}

void mainFile(const std::string& path) {
    std::ifstream in(path);
    std::string query;
    while (std::getline(in, query)) {
        std::cout << "Initial query:\n" << query << std::endl;
        YY_BUFFER_STATE state;
        if (!(state = yy_scan_bytes(query.c_str(), strcspn(query.c_str(), "\n")))) {
            continue;
        }
        Query ret;
        std::cout << "======================\n" << std::endl;
        if (yyparse(&ret) == 0) {
            printQueryReport(ret);
        }
        yy_delete_buffer(state);
    }
}

// format is:
// ./main - interactive mode
// ./main <PATH>
int main(int argc, char **argv) {
    if (argc != 1) {
        assert(argc == 2);
    }
    if (argc == 1) {
        mainInteractive();
    } else {
        mainFile(argv[1]);
    }
    return 0;
}

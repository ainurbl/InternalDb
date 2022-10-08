#include <cstdio>
#include <map>
#include <functional>

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

int main() {
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

    return 0;
}

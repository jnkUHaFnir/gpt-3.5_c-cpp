#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <r_core.h>
#include <r_anal.h>
#include <r_bin.h>

void analyze_function(char* binary_path, char* function_name) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);
    r_core_cmd(core, "aa"); // run 'aa' command to analyze all functions

    char cmd[100];
    snprintf(cmd, sizeof(cmd), "pdf @ %s", function_name); // generate graph for specific function
    r_core_cmd(core, cmd);

    r_core_free(core);
}

void analyze_cfg(const char* binary_path, const char* function_name) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);

    char cmd[100];
    snprintf(cmd, sizeof(cmd), "pdf @ %s", function_name);
    r_core_cmd(core, cmd);

    RAnalFunction* func = r_anal_get_fcn_in_core(core->anal, core->offset);
    if (func == NULL) {
        printf("Error: function not found\n");
        return;
    }

    RGraphNode* node = r_graph_new_node("entry");
    r_graph_add_node(func->graph, node);

    for (int i = 0; i < func->ninstr; i++) {
        RAnalInstruction* instr = &func->instr[i];

        if (instr->type & R_ANAL_INSTR_TYPE_CJMP) {
            int offset = instr->jump;
            RAnalFunction* target_func = r_anal_get_fcn_in_core(core->anal, offset);
            if (target_func != NULL) {
                char name[100];
                snprintf(name, sizeof(name), "fcn_%08x", target_func->addr);
                RGraphNode* target_node = r_graph_new_node(name);
                r_graph_add_node(func->graph, target_node);
                r_graph_add_edge(node, target_node, NULL);
            }
        }
        node = r_graph_new_node(instr->mnemonic);
        r_graph_add_node(func->graph, node);
        r_graph_add_edge(node, node + 1, NULL);
    }

    r_core_free(core);
}

void analyze_strings(const char* binary_path) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);

    char cmd[100];
    snprintf(cmd, sizeof(cmd), "iz"); // run 'iz' command to list strings
    RCons* cons = r_cons_new();
    RIO* io = r_io_new();
    r_io_use(io, cons);
    r_cons_set_io(cons, io);
    r_core_cmd_exec(core, cmd, io, io);

    RList* list = r_list_newf((RListFree)free);
    r_list_push(list, strdup("string"));
    r_list_push(list, strdup("type"));

    RList* strings = r_anal_strings(core->anal);
    RListIter* iter;
    RAnalString* str;

    r_list_foreach(strings, iter, str) {
        if (strcmp(str->type, "ascii") == 0) {
            RList* row = r_list_newf((RListFree)free);
            r_list_push(row, strdup(str->string));
            r_list_push(row, strdup(str->type));
            r_list_push(list, row);
        }
    }

    r_cons_printf(cons, "%s", r_list_to_json(list, 0));
    r_list_free(list);
    r_list_free(strings);

    r_core_free(core);
}

void analyze_coverage(const char* binary_path, const char* input_path) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);

    char cmd[100];
    snprintf(cmd, sizeof(cmd), "aa; agfd"); // run 'aa' and 'agfd' commands to analyze and generate function database
    r_core_cmd(core, cmd);

    r_core_file_fd_open(core, input_path, "r", 0); // open input file for reading

    RList* hits = r_list_newf((RListFree)free);
    RAnalFunction* func;
    RAnalBlock* block;
    RAnalHit* hit;

    r_list_foreach(core->anal->fcns, iter, func) {
        r_anal_function_coverage(core->anal, func); // compute coverage for function
        r_list_foreach(func->blocks, iter, block) {
            r_anal_block_coverage(core->anal, block); // compute coverage for block
            r_list_foreach(block->hits, iter, hit) {
                if (hit->covered) {
                    char* name = r_str_newf("%s+%lx", func->name, hit->offset);
                    r_list_push(hits, name); // add covered block to list of hits
                }
            }
        }
    }

    r_core_file_fd_close(core); // close input file

    RList* list = r_list_newf((RListFree)free);
    r_list_push(list, strdup("hit"));

    r_list_foreach(hits, iter, hit) {
        RList* row = r_list_newf((RListFree)free);
        r_list_push(row, strdup(hit));
        r_list_push(list, row);
    }

    r_cons_printf(core->cons, "%s", r_list_to_json(list, 0));

    r_list_free(list);
    r_list_free(hits);
    r_core_free(core);
}

void analyze_functions(const char* binary_path) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);

    char cmd[100];
    snprintf(cmd, sizeof(cmd), "aaa; aac"); // run 'aaa' and 'aac' commands to analyze and generate call graph
    r_core_cmd(core, cmd);

    RAnalFunction* func;
    RGraphNode* node;

    RGraph* graph = r_anal_cg(core->anal);
    r_graph_foreach_node(graph, node) {
        func = node->data;
        printf("Function %s at 0x%llx\n", func->name, func->addr);
        RListIter* iter;
        RAnalFunction* callee;
        r_list_foreach(func->callee, iter, callee) {
            printf("\tCalls %s at 0x%llx\n", callee->name, callee->addr);
        }
    }

    r_core_free(core);
}

void analyze_imports(const char* binary_path) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);

    char cmd[100];
    snprintf(cmd, sizeof(cmd), "iij"); // run 'iij' command to list imported functions
    RCons* cons = r_cons_new();
    RIO* io = r_io_new();
    r_io_use(io, cons);
    r_cons_set_io(cons, io);
    r_core_cmd_exec(core, cmd, io, io);

    RList* imports = r_bin_imports(core->bin);
    RListIter* iter;
    RBinImport* import;

    RList* list = r_list_newf((RListFree)free);
    r_list_push(list, strdup("name"));

    r_list_foreach(imports, iter, import) {
        if (import->name != NULL) {
            RList* row = r_list_newf((RListFree)free);
            r_list_push(row, strdup(import->name));
            r_list_push(list, row);
        }
    }

    r_cons_printf(cons, "%s", r_list_to_json(list, 0));

    r_list_free(list);
    r_list_free(imports);
    r_core_free(core);
}

void analyze_prologue_epilogue(const char* binary_path) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);

    RAnalFunction* func;
    RAnalOp* op;
    RBinSymbol* sym;

    RList* prologue_list = r_list_newf((RListFree)free);
    r_list_push(prologue_list, strdup("function_name"));
    r_list_push(prologue_list, strdup("function_address"));
    r_list_push(prologue_list, strdup("prologue_size"));
    r_list_push(prologue_list, strdup("epilogue_size"));

    RList* functions = r_bin_get_functions(core->bin);
    RListIter* iter;
    r_list_foreach(functions, iter, sym) {
        if (sym->type != R_BIN_TYPE_FUNC) {
            continue;
        }

        func = r_anal_get_function_at(core->anal, sym->vaddr, R_ANAL_ARCH_X86_64);
        if (!func) {
            continue;
        }

        RList* prologue_row = r_list_newf((RListFree)free);
        r_list_push(prologue_row, strdup(sym->name));
        r_list_push(prologue_row, r_str_newf("%llx", sym->vaddr));

        int prologue_size = 0;
        int epilogue_size = 0;

        r_anal_function_op_foreach(core->anal, func, op) {
            if (op->type != R_ANAL_OP_TYPE_MOV && op->type != R_ANAL_OP_TYPE_PUSH) {
                break;
            }
            prologue_size += op->size;
        }

        int last_offset = 0;
        r_anal_function_op_foreach(core->anal, func, op) {
            if (op->type == R_ANAL_OP_TYPE_LEAVE || op->type == R_ANAL_OP_TYPE_RET) {
                if (last_offset != 0) {
                    epilogue_size += op->offset - last_offset;
                }
                break;
            }
            last_offset = op->offset;
        }

        r_list_push(prologue_row, r_str_newf("%d", prologue_size));
        r_list_push(prologue_row, r_str_newf("%d", epilogue_size));

        r_list_push(prologue_list, prologue_row);
    }

    r_cons_printf(core->cons, "%s", r_list_to_csv(prologue_list, 0));

    r_list_free(prologue_list);
    r_list_free(functions);
    r_core_free(core);
}

void analyze_control_flow(const char* binary_path, const char* function_name) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);

    RAnalFunction* func = r_anal_get_function_by_name(core->anal, function_name);
    if (!func) {
        r_cons_printf(core->cons, "Error: function not found\n");
        return;
    }

    r_anal_function_disasm(core->anal, func->addr, func->size, R_ANAL_DISASM_DEPTH_ALL);

    RGraphNode* node;
    RList* nodes = r_graph_new_list();

    r_graph_build(core->anal->graph, (const ut64*)&func->addr, 1, nodes);

    RListIter* iter;
    r_list_foreach(nodes, iter, node) {
        RAnalOp* op = node->data;
        if (!op) {
            continue;
        }

        if (op->type == R_ANAL_OP_TYPE_CALL || op->type == R_ANAL_OP_TYPE_JMP) {
            RList* children = r_list_newf((RListFree)free);
            r_list_push(children, strdup(r_str_newf("0x%llx", op->jump)));
            r_graph_foreach_edge(core->anal->graph, node, node, edge) {
                RAnalOp* child_op = node->data;
                if (child_op->offset == edge->addr) {
                    r_list_push(children, strdup(child_op->mnemonic));
                }
            }
            r_graph_node_set_children(node, children);
        }
    }

    r_cons_printf(core->cons, "%s", r_graph_to_json(core->anal->graph, nodes, 0));

    r_list_free(nodes);
    r_core_free(core);
}

typedef struct {
    char* name;
    int count;
} CryptoFunction;

void analyze_cryptography(const char* binary_path) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);

    CryptoFunction* crypto_functions = malloc(sizeof(CryptoFunction) * 4);
    crypto_functions[0] = (CryptoFunction){ .name = "AES", .count = 0 };
    crypto_functions[1] = (CryptoFunction){ .name = "RSA", .count = 0 };
    crypto_functions[2] = (CryptoFunction){ .name = "MD5", .count = 0 };
    crypto_functions[3] = (CryptoFunction){ .name = "SHA", .count = 0 };

    RList* functions = r_bin_get_functions(core->bin);
    RListIter* iter;
    RBinSymbol* sym;

    r_list_foreach(functions, iter, sym) {
        if (sym->type != R_BIN_TYPE_FUNC) {
            continue;
        }

        RAnalFunction* func = r_anal_get_function_at(core->anal, sym->vaddr, R_ANAL_ARCH_DEFAULT);
        if (!func) {
            continue;
        }

        RAnalOp* op;
        r_anal_function_op_foreach(core->anal, func, op) {
            if (op->type == R_ANAL_OP_TYPE_CALL) {
                const char* function_name = r_anal_get_op_str(core->anal, op, R_ANAL_OP_STR_CANON);
                if (strstr(function_name, "AES_") != NULL || strstr(function_name, "AES_CBC") != NULL || strstr(function_name, "AES_CTR") != NULL) {
                    crypto_functions[0].count++;
                }
                else if (strstr(function_name, "RSA_") != NULL || strstr(function_name, "RSA_PUB_ENC") != NULL || strstr(function_name, "RSA_PRV_DEC") != NULL) {
                    crypto_functions[1].count++;
                }
                else if (strstr(function_name, "MD5_") != NULL) {
                    crypto_functions[2].count++;
                }
                else if (strstr(function_name, "SHA_") != NULL) {
                    crypto_functions[3].count++;
                }
            }
        }
    }

    r_cons_printf(core->cons, "| Function | Count |\n");
    r_cons_printf(core->cons, "| -------- | ----- |\n");

    for (int i = 0; i < 4; i++) {
        r_cons_printf(core->cons, "| %s | %d |\n", crypto_functions[i].name, crypto_functions[i].count);
    }

    free(crypto_functions);
    r_list_free(functions);
    r_core_free(core);
}

typedef struct {
    char* name;
    int count;
} Variable;

typedef struct {
    char* source;
    char* sink;
    int count;
} DataFlow;

void analyze_data_flow(const char* binary_path, const char* function_name) {
    RCore* core = r_core_new();
    r_core_file_open(core, binary_path, NULL);

    RAnalFunction* func = r_anal_get_function_by_name(core->anal, function_name);
    if (!func) {
        r_cons_printf(core->cons, "Error: function not found\n");
        return;
    }

    r_anal_function_disasm(core->anal, func->addr, func->size, R_ANAL_DISASM_DEPTH_ALL);

    RList* variables = r_list_newf((RListFree)free);
    r_list_push(variables, strdup("Variable"));
    r_list_push(variables, strdup("Count"));

    RList* data_flows = r_list_newf((RListFree)free);
    r_list_push(data_flows, strdup("Source"));
    r_list_push(data_flows, strdup("Sink"));
    r_list_push(data_flows, strdup("Count"));

    RGraphNode* node;
    RList* nodes = r_graph_new_list();

    r_graph_build(core->anal->graph, (const ut64*)&func->addr, 1, nodes);

    RListIter* iter;
    r_list_foreach(nodes, iter, node) {
        RAnalOp* op = node->data;
        if (!op) {
            continue;
        }

        if (op->type == R_ANAL_OP_TYPE_MOV) {
            const char* dest_var = r_anal_get_op_str(core->anal, op, R_ANAL_OP_STR_DST);
            const char* src_var = r_anal_get_op_str(core->anal, op, R_ANAL_OP_STR_SRC);

            if (dest_var && src_var) {
                int found_dest = 0;
                int found_src = 0;

                for (int i = 1; i < r_list_length(variables); i++) {
                    char* var_name = r_list_get(variables, i);
                    if (strcmp(dest_var, var_name) == 0) {
                        int count = *(int*)r_list_get(variables, i + 1);
                        count++;
                        r_list_set(variables, i + 1, &count);
                        found_dest = 1;
                    }
                    else if (strcmp(src_var, var_name) == 0) {
                        int count = *(int*)r_list_get(variables, i + 1);
                        count++;
                        r_list_set(variables, i + 1, &count);
                        found_src = 1;
                    }
                }

                if (!found_dest) {
                    Variable* var = malloc(sizeof(Variable));
                    var->name = strdup(dest_var);
                    var->count = 1;
                    r_list_push(variables, var->name);
                    r_list_push(variables, &var->count);
                }

                if (!found_src) {
                    Variable* var = malloc(sizeof(Variable));
                    var->name = strdup(src_var);
                    var->count = 1;
                    r_list_push(variables, var->name);
                    r_list_push(variables, &var->count);
                }
            }
        }
    }
}
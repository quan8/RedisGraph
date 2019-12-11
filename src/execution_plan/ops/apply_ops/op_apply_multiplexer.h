/*
 * Copyright 2018-2019 Redis Labs Ltd. and Contributors
 *
 * This file is available under the Redis Labs Source Available License Agreement
 */

#pragma once

#include "../op.h"
#include "../op_argument.h"
#include "../../execution_plan.h"
#include "../op_filter.h"

/* ApplyMultiplexer operation tests for condition satisfaction over multiple execution plan
 * branches. The branches can be a simple filter operation, SemiApply operations,
 * or ApplyMultiplexer operations. The logic applied by the operation is defined by the
 * boolean operators OR and AND.
 * ORApplyMultiplexer: Starts by pulling on the main execution plan branch,
 * for each record received it tries to get a record from the filter branch, if exists. If
 * the filter is not exists, or did not return any record, the operation will check each of its branches
 * until a record is found. If no data is produced from any branch,it will try to fetch a new data point
 * from the main execution plan branch, otherwise the main execution plan branch record is passed onward.
 * ANDApplyMultiplexer: Starts by pulling on the main execution plan branch,
 * for each record received it tries to get a record from the filter branch, if exists. If
 * the filter is not exists, or the filter returned a record, the operation will check each of its branches
 * until a record is found. If one branch did not produced any data, the operation will try to fetch
 * a new data point from the main execution plan branch, otherwise the main execution plan branch
 * record is passed onward.*/

struct OpApplyMultiplexer;

typedef Record(*ApplyLogic)(struct OpApplyMultiplexer *);

typedef struct OpApplyMultiplexer {
	OpBase op;
	Record r;                       // Bound branch record.
	OpBase *bound_branch;           // Bound branch root;
	OpFilter **filters;             // Optional filters.
	Argument **filters_arguments;   // Optional filter taps;
	OpBase **branches;              // branches roots;
	Argument **branches_arguments;  // Branches taps.
	ApplyLogic apply_func;          // Which apply method to invoke (OR or AND multiplexion).
	AST_Operator boolean_operator;  // Defines the operation logic.
} OpApplyMultiplexer;

OpBase *NewApplyMultiplexerOp(ExecutionPlan *plan, AST_Operator boolean_operator);

/* Sets the main execution plan branch. In case this operation is called from ApplyMultiplexer operation
 * this branch will be an argument operation. */
void OpApplyMultiplexer_SetExecutionPlanBranch(OpApplyMultiplexer *apply_multiplexer,
											   OpBase *execution_plan_root);

void OpApplyMultiplexer_AddBranch(OpApplyMultiplexer *apply_multiplexer, OpBase *branch_root);

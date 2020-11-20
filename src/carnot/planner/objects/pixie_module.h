#pragma once
#include <memory>
#include <string>
#include <vector>

#include "src/carnot/planner/compiler_state/compiler_state.h"
#include "src/carnot/planner/objects/funcobject.h"

namespace pl {
namespace carnot {
namespace planner {
namespace compiler {

class PixieModule : public QLObject {
 public:
  static constexpr TypeDescriptor PixieModuleType = {
      /* name */ "px",
      /* type */ QLObjectType::kPLModule,
  };
  static StatusOr<std::shared_ptr<PixieModule>> Create(
      IR* graph, CompilerState* compiler_state, ASTVisitor* ast_visitor,
      bool func_based_exec = false, const absl::flat_hash_set<std::string>& reserved_names = {});

  // Constant for the modules.
  inline static constexpr char kPixieModuleObjName[] = "px";
  inline static constexpr char kOldPixieModuleObjName[] = "pl";

  // Operator function names.
  inline static constexpr char kDataframeOpID[] = "DataFrame";
  inline static constexpr char kDisplayOpID[] = "display";
  inline static constexpr char kDisplayOpDocstring[] = R"doc(
  Outputs the data from the engine.

  Writes the data to the output stream. Disabled if executing using Vis functions.
  If you want to still see data when using vis fucntions, use `px.debug`.

  :topic: dataframe_ops

  Args:
    out (px.DataFrame): The DataFrame to write out to the output stream.
    name (string): The output table name for the DataFrame. If not set, then
      will be 'output'. If the name is duplicated across all written tables, we
      suffix with `_1`, incrementing for every duplicate.
  )doc";

  inline static constexpr char kDebugTablePrefix[] = "_";
  inline static constexpr char kDebugOpID[] = "debug";
  inline static constexpr char kDebugOpDocstring[] = R"doc(
  Outputs the data from the engine as a debug table

  Writes the data to the output stream, prefixing the name with `_`. Unlike `px.display`
  if executing the script with Vis functions, this will still write to the output table.

  :topic: dataframe_ops

  Args:
    out (px.DataFrame): The DataFrame to write out to the output stream.
    name (string): The output table name for the DataFrame. If not set, then
      will be 'output'. If the name is duplicated across all written tables, we
      suffix with `_1`, incrementing for every duplicate.
  )doc";

  // Compile time functions
  inline static constexpr char kNowOpID[] = "now";
  inline static constexpr char kNowOpDocstring[] = R"doc(
  Get the current time.

  :topic: compile_time_fn

  Returns:
    px.Time: The current time as defined at the start of compilation.

  )doc";
  inline static constexpr char kEqualsAnyID[] = "equals_any";
  inline static constexpr char kEqualsAnyDocstring[] = R"doc(
  Returns true if the value is in the list.

  Check equality of the input value with every element of a list.

  Examples:
    df.val = px.equals_any(df.remote_addr, ['10.0.0.1', '10.0.0.2'])

  :topic: compile_time_fn

  Args:
    value (px.Expr): The value to compare.
    comparisons (List[px.Expr]): The list of values to check equality to the value.

  Returns:
    px.Expr: An expression that evaluates to true if the value is found in the list.

  )doc";

  inline static constexpr char kUInt128ConversionID[] = "uint128";
  inline static constexpr char kUInt128ConversionDocstring[] = R"doc(
  Parse the UUID string into a UInt128.

  Parse the UUID string of canonical textual representation into a 128bit
  integer (ie "123e4567-e89b-12d3-a456-426614174000"). Errors out if the string
  is not the correct format.

  Examples:
    val = px.uint128("123e4567-e89b-12d3-a456-426614174000")

  :topic: compile_time_fn

  Args:
    uuid (string): the uuid in canoncial uuid4 format ("123e4567-e89b-12d3-a456-426614174000")
  Returns:
    uint128: The uuid as a uint128.
  )doc";
  inline static constexpr char kMakeUPIDID[] = "make_upid";
  inline static constexpr char kMakeUPIDDocstring[] = R"doc(
  Create a UPID from its components to represent a process.

  Creates a UPID object from asid, pid, and time started in nanoseconds. UPID stands for
  unique PID and is a Pixie concept to ensure tracked processes are unique in time and across
  nodes.

  Note: Creating this value from scratch might be very difficult, espeically given the nanosecond timestamp.
  It's probably only useful if you find the UPID printed out as it's consitituent components.

  In most situations, you might find that `px.uint128` is a better option as we often render UPID as uuid.

  Examples:
    val = px.make_upid(123, 456, 1598419816000000)

  :topic: compile_time_fn

  Args:
    asid (int): The ID of the node, according to the Pixie metadata service. Stands for Agent short ID.
    pid (int): The PID of the process on the node.
    ts_ns (int): The start time of the process in unix time.
  Returns:
    px.UPID: The represented UPID.
  )doc";
  inline static constexpr char kAbsTimeOpID[] = "strptime";
  inline static constexpr char kAbsTimeDocstring[] = R"doc(
  Parse a datestring into a px.Time.

  Parse a datestring using a standard time format template into an internal time representation.
  The format must follow the C strptime format, outlined in this document:
  https://pubs.opengroup.org/onlinepubs/009695399/functions/strptime.html

  Examples:
    time = px.strptime("2020-03-12 19:39:59 -0200", "%Y-%m-%d %H:%M:%S %z")

  :topic: compile_time_fn

  Args:
    date_string (string): The time as a string, should match the format object.
    format (string): The string format according to the C strptime format
      https://pubs.opengroup.org/onlinepubs/009695399/functions/strptime.html
  Returns:
    px.Time: The time value represented in the data.
  )doc";

  inline static constexpr char kScriptReferenceID[] = "script_reference";
  inline static constexpr char kScriptReferenceDocstring[] = R"doc(
  Create a reference to a PxL script.

  Create a reference to a PxL script with specified script arguments.
  These values are displayed in the UI as a clickable link to execute that PxL script.

  Examples:
    df.script = px.script_reference(df.namespace, 'px/namespace', {
      'namespace': df.namespace,
      'start_time': '-5m'
    })

  Args:
    label (string): A value containing the label text for the output deep link.
    script (string): The script ID to execute, such as 'px/namespace'.
    args (dictionary): A dictionary containing the script argument values.

  Returns:
    string: A stringified JSON representing the script, shown in the UI as a link.

  )doc";

  inline static constexpr char kTimeFuncDocstringTpl[] = R"doc(
  Gets the specified number of $0.

  Examples:
    # Returns 2 $0.
    time = px.$0(2)

  :topic: compile_time_fn

  Args:
    unit (int): The number of $0 to render.
  Returns:
    px.Duration: Duration representing `unit` $0.
  )doc";
  static const constexpr char* const kTimeFuncs[] = {"minutes", "hours",        "seconds",
                                                     "days",    "microseconds", "milliseconds"};

  // Type constants.
  inline static constexpr char kTimeTypeName[] = "Time";
  inline static constexpr char kContainerTypeName[] = "Container";
  inline static constexpr char kNamespaceTypeName[] = "Namespace";
  inline static constexpr char kNodeTypeName[] = "Node";
  inline static constexpr char kPodTypeName[] = "Pod";
  inline static constexpr char kServiceTypeName[] = "Service";
  inline static constexpr char kBytesTypeName[] = "Bytes";
  inline static constexpr char kDurationNSTypeName[] = "DurationNanos";
  inline static constexpr char kUPIDTypeName[] = "UPID";
  inline static constexpr char kPercentTypeName[] = "Percent";

  // Submodules of Px.
  inline static constexpr char kVisAttrID[] = "vis";

 protected:
  explicit PixieModule(IR* graph, CompilerState* compiler_state, ASTVisitor* ast_visitor,
                       bool func_based_exec, const absl::flat_hash_set<std::string>& reserved_names)
      : QLObject(PixieModuleType, ast_visitor),
        graph_(graph),
        compiler_state_(compiler_state),
        func_based_exec_(func_based_exec),
        reserved_names_(reserved_names) {}
  Status Init();
  Status RegisterUDFFuncs();
  Status RegisterUDTFs();
  Status RegisterCompileTimeFuncs();
  Status RegisterCompileTimeUnitFunction(std::string name);
  Status RegisterTypeObjs();

 private:
  IR* graph_;
  CompilerState* compiler_state_;
  absl::flat_hash_set<std::string> compiler_time_fns_;
  const bool func_based_exec_;
  absl::flat_hash_set<std::string> reserved_names_;
};

/**
 * @brief Implements the pl.display() logic.
 *
 */
class DisplayHandler {
 public:
  static StatusOr<QLObjectPtr> Eval(IR* graph, CompilerState* compiler_state,
                                    const pypa::AstPtr& ast, const ParsedArgs& args,
                                    ASTVisitor* visitor);
};

/**
 * @brief Implements the px.display() logic, when doing function based execution.
 */
class NoopDisplayHandler {
 public:
  static StatusOr<QLObjectPtr> Eval(IR* graph, CompilerState* compiler_state,
                                    const pypa::AstPtr& ast, const ParsedArgs& args,
                                    ASTVisitor* visitor);
};

/**
 * @brief Implements the px.debug() logic.
 */
class DebugDisplayHandler {
 public:
  static StatusOr<QLObjectPtr> Eval(IR* graph, CompilerState* compiler_state,
                                    const absl::flat_hash_set<std::string>& reserved_names,
                                    const pypa::AstPtr& ast, const ParsedArgs& args,
                                    ASTVisitor* visitor);
};

/**
 * @brief Implements the pl.now(), pl.minutes(), pl.hours(), etc.
 *
 */
class CompileTimeFuncHandler {
 public:
  static StatusOr<QLObjectPtr> NowEval(CompilerState* compiler_state, IR* graph,
                                       const pypa::AstPtr& ast, const ParsedArgs& args,
                                       ASTVisitor* visitor);
  static StatusOr<QLObjectPtr> TimeEval(IR* graph, std::string name, const pypa::AstPtr& ast,
                                        const ParsedArgs& args, ASTVisitor* visitor);
  static StatusOr<QLObjectPtr> UInt128Conversion(IR* graph, const pypa::AstPtr& ast,
                                                 const ParsedArgs& args, ASTVisitor* visitor);
  static StatusOr<QLObjectPtr> UPIDConstructor(IR* graph, const pypa::AstPtr& ast,
                                               const ParsedArgs& args, ASTVisitor* visitor);
  static StatusOr<QLObjectPtr> AbsTime(IR* graph, const pypa::AstPtr& ast, const ParsedArgs& args,
                                       ASTVisitor* visitor);
  static StatusOr<QLObjectPtr> EqualsAny(IR* graph, const pypa::AstPtr& ast, const ParsedArgs& args,
                                         ASTVisitor* visitor);
  static StatusOr<QLObjectPtr> ScriptReference(IR* graph, const pypa::AstPtr& ast,
                                               const ParsedArgs& args, ASTVisitor* visitor);
};

/**
 * @brief Implements the udf logic.
 *
 */
class UDFHandler {
 public:
  static StatusOr<QLObjectPtr> Eval(IR* graph, std::string name, const pypa::AstPtr& ast,
                                    const ParsedArgs& args, ASTVisitor* visitor);
};

/**
 * @brief Implements the logic that implements udtf_source_specification.
 *
 */
class UDTFSourceHandler {
 public:
  static StatusOr<QLObjectPtr> Eval(IR* graph, const udfspb::UDTFSourceSpec& udtf_source_spec,
                                    const pypa::AstPtr& ast, const ParsedArgs& args,
                                    ASTVisitor* visitor);

 private:
  static StatusOr<ExpressionIR*> EvaluateExpression(IR* graph, IRNode* arg_node,
                                                    const udfspb::UDTFSourceSpec::Arg& arg);
};

// Helper function to add a result sink to an IR.
inline Status AddResultSink(IR* graph, const pypa::AstPtr& ast, std::string_view out_name,
                            OperatorIR* parent_op, std::string_view result_addr,
                            std::string_view result_ssl_targetname) {
  // It's a bit more concise to do column selection using a keep:
  // px.display(df[['cols', 'to', 'keep']])
  // than passing cols as a separate param:
  // px.display(df, cols=['cols', 'to', 'keep'])
  // So we don't currently support passing those output columns as an argument to display.
  std::vector<std::string> columns;
  PL_ASSIGN_OR_RETURN(
      auto sink, graph->CreateNode<GRPCSinkIR>(ast, parent_op, std::string(out_name), columns));
  sink->SetDestinationAddress(std::string(result_addr));
  sink->SetDestinationSSLTargetName(std::string(result_ssl_targetname));
  return Status::OK();
}

}  // namespace compiler
}  // namespace planner
}  // namespace carnot
}  // namespace pl

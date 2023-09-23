
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include <algorithm>
#include <map>
#include <queue>
#include <stack>
#include "cgen.h"
#include "cgen_gc.h"
#include <string>
#include <tuple>

extern void emit_string_constant(ostream &str, char *s);
extern int cgen_debug;
int label_index = 0;
std::map<Symbol, CgenNodeP> sym_node;

//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
  arg = idtable.add_string("arg");
  arg2 = idtable.add_string("arg2");
  Bool = idtable.add_string("Bool");
  concat = idtable.add_string("concat");
  cool_abort = idtable.add_string("abort");
  copy = idtable.add_string("copy");
  Int = idtable.add_string("Int");
  in_int = idtable.add_string("in_int");
  in_string = idtable.add_string("in_string");
  IO = idtable.add_string("IO");
  length = idtable.add_string("length");
  Main = idtable.add_string("Main");
  main_meth = idtable.add_string("main");
  //   _no_class is a symbol that can't be the name of any
  //   user-defined class.
  No_class = idtable.add_string("_no_class");
  No_type = idtable.add_string("_no_type");
  Object = idtable.add_string("Object");
  out_int = idtable.add_string("out_int");
  out_string = idtable.add_string("out_string");
  prim_slot = idtable.add_string("_prim_slot");
  self = idtable.add_string("self");
  SELF_TYPE = idtable.add_string("SELF_TYPE");
  Str = idtable.add_string("String");
  str_field = idtable.add_string("_str_field");
  substr = idtable.add_string("substr");
  type_name = idtable.add_string("type_name");
  val = idtable.add_string("_val");
}

static char *gc_init_names[] =
    {"_NoGC_Init", "_GenGC_Init", "_ScnGC_Init"};
static char *gc_collect_names[] =
    {"_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect"};

//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

void program_class::cgen(ostream &os)
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  initialize_constants();
  CgenClassTable *codegen_classtable = new CgenClassTable(classes, os);

  os << "\n# end of generated code\n";
}

//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_load(char *dest_reg, int offset, char *source_reg, ostream &s)
{
  s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")"
    << endl;
}

static void emit_store(char *source_reg, int offset, char *dest_reg, ostream &s)
{
  s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")"
    << endl;
}

static void emit_load_imm(char *dest_reg, int val, ostream &s)
{
  s << LI << dest_reg << " " << val << endl;
}

static void emit_load_address(char *dest_reg, char *address, ostream &s)
{
  s << LA << dest_reg << " " << address << endl;
}

static void emit_partial_load_address(char *dest_reg, ostream &s)
{
  s << LA << dest_reg << " ";
}

static void emit_load_bool(char *dest, const BoolConst &b, ostream &s)
{
  emit_partial_load_address(dest, s);
  b.code_ref(s);
  s << endl;
}

static void emit_load_string(char *dest, StringEntry *str, ostream &s)
{
  emit_partial_load_address(dest, s);
  str->code_ref(s);
  s << endl;
}

static void emit_load_int(char *dest, IntEntry *i, ostream &s)
{
  emit_partial_load_address(dest, s);
  i->code_ref(s);
  s << endl;
}

static void emit_move(char *dest_reg, char *source_reg, ostream &s)
{
  s << MOVE << dest_reg << " " << source_reg << endl;
}

static void emit_neg(char *dest, char *src1, ostream &s)
{
  s << NEG << dest << " " << src1 << endl;
}

static void emit_add(char *dest, char *src1, char *src2, ostream &s)
{
  s << ADD << dest << " " << src1 << " " << src2 << endl;
}

static void emit_addu(char *dest, char *src1, char *src2, ostream &s)
{
  s << ADDU << dest << " " << src1 << " " << src2 << endl;
}

static void emit_addiu(char *dest, char *src1, int imm, ostream &s)
{
  s << ADDIU << dest << " " << src1 << " " << imm << endl;
}

static void emit_div(char *dest, char *src1, char *src2, ostream &s)
{
  s << DIV << dest << " " << src1 << " " << src2 << endl;
}

static void emit_mul(char *dest, char *src1, char *src2, ostream &s)
{
  s << MUL << dest << " " << src1 << " " << src2 << endl;
}

static void emit_sub(char *dest, char *src1, char *src2, ostream &s)
{
  s << SUB << dest << " " << src1 << " " << src2 << endl;
}

static void emit_sll(char *dest, char *src1, int num, ostream &s)
{
  s << SLL << dest << " " << src1 << " " << num << endl;
}

static void emit_jalr(char *dest, ostream &s)
{
  s << JALR << "\t" << dest << endl;
}

static void emit_jal(char *address, ostream &s)
{
  s << JAL << address << endl;
}

static void emit_return(ostream &s)
{
  s << RET << endl;
}

static void emit_gc_assign(ostream &s)
{
  s << JAL << "_GenGC_Assign" << endl;
}

static void emit_disptable_ref(Symbol sym, ostream &s)
{
  s << sym << DISPTAB_SUFFIX;
}

static void emit_init_ref(Symbol sym, ostream &s)
{
  s << sym << CLASSINIT_SUFFIX;
}

static void emit_label_ref(int l, ostream &s)
{
  s << "label" << l;
}

static void emit_protobj_ref(Symbol sym, ostream &s)
{
  s << sym << PROTOBJ_SUFFIX;
}

static void emit_method_ref(Symbol classname, Symbol methodname, ostream &s)
{
  s << classname << METHOD_SEP << methodname;
}

static void emit_label_def(int l, ostream &s)
{
  emit_label_ref(l, s);
  s << ":" << endl;
}

static void emit_beqz(char *source, int label, ostream &s)
{
  s << BEQZ << source << " ";
  emit_label_ref(label, s);
  s << endl;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s)
{
  s << BEQ << src1 << " " << src2 << " ";
  emit_label_ref(label, s);
  s << endl;
}

static void emit_bne(char *src1, char *src2, int label, ostream &s)
{
  s << BNE << src1 << " " << src2 << " ";
  emit_label_ref(label, s);
  s << endl;
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s)
{
  s << BLEQ << src1 << " " << src2 << " ";
  emit_label_ref(label, s);
  s << endl;
}

static void emit_blt(char *src1, char *src2, int label, ostream &s)
{
  s << BLT << src1 << " " << src2 << " ";
  emit_label_ref(label, s);
  s << endl;
}

static void emit_blti(char *src1, int imm, int label, ostream &s)
{
  s << BLT << src1 << " " << imm << " ";
  emit_label_ref(label, s);
  s << endl;
}

static void emit_bgti(char *src1, int imm, int label, ostream &s)
{
  s << BGT << src1 << " " << imm << " ";
  emit_label_ref(label, s);
  s << endl;
}

static void emit_branch(int l, ostream &s)
{
  s << BRANCH;
  emit_label_ref(l, s);
  s << endl;
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
static void emit_push(char *reg, ostream &str)
{
  emit_store(reg, 0, SP, str);
  emit_addiu(SP, SP, -4, str);
}

//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char *dest, char *source, ostream &s)
{
  emit_load(dest, DEFAULT_OBJFIELDS, source, s);
}

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, ostream &s)
{
  emit_store(source, DEFAULT_OBJFIELDS, dest, s);
}

static void emit_test_collector(ostream &s)
{
  emit_push(ACC, s);
  emit_move(ACC, SP, s);  // stack end
  emit_move(A1, ZERO, s); // allocate nothing
  s << JAL << gc_collect_names[cgen_Memmgr] << endl;
  emit_addiu(SP, SP, 4, s);
  emit_load(ACC, 0, SP, s);
}

static void emit_gc_check(char *source, ostream &s)
{
  if (source != (char *)A1)
    emit_move(A1, source, s);
  s << JAL << "_gc_check" << endl;
}

///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream &s)
{
  s << STRCONST_PREFIX << index;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream &s, int stringclasstag)
{
  IntEntryP lensym = inttable.add_int(len);

  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);
  s << LABEL                                                              // label
    << WORD << stringclasstag << endl                                     // tag
    << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len + 4) / 4) << endl // size
    << WORD;

  s << Str << DISPTAB_SUFFIX;
  /***** Add dispatch information for class String ******/

  s << endl; // dispatch table
  s << WORD;
  lensym->code_ref(s);
  s << endl;                    // string length
  emit_string_constant(s, str); // ascii string
  s << ALIGN;                   // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the
// stringtable.
//
void StrTable::code_string_table(ostream &s, int stringclasstag)
{
  for (List<StringEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s, stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(ostream &s)
{
  s << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);
  s << LABEL                                           // label
    << WORD << intclasstag << endl                     // class tag
    << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl // object size
    << WORD;

  /***** Add dispatch information for class Int ******/
  s << Int << DISPTAB_SUFFIX;

  s << endl;                // dispatch table
  s << WORD << str << endl; // integer value
}

//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(ostream &s, int intclasstag)
{
  for (List<IntEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s, intclasstag);
}

//
// Bools
//
BoolConst::BoolConst(int i) : val(i) { assert(i == 0 || i == 1); }

void BoolConst::code_ref(ostream &s) const
{
  s << BOOLCONST_PREFIX << val;
}

//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream &s, int boolclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);
  s << LABEL                                            // label
    << WORD << boolclasstag << endl                     // class tag
    << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl // object size
    << WORD;

  /***** Add dispatch information for class Bool ******/
  s << Bool << DISPTAB_SUFFIX;
  s << endl;                // dispatch table
  s << WORD << val << endl; // value (0 or 1)
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data()
{
  Symbol main = idtable.lookup_string(MAINNAME);
  Symbol string = idtable.lookup_string(STRINGNAME);
  Symbol integer = idtable.lookup_string(INTNAME);
  Symbol boolc = idtable.lookup_string(BOOLNAME);

  str << "\t.data\n"
      << ALIGN;
  //
  // The following global names must be defined first.
  //
  str << GLOBAL << CLASSNAMETAB << endl;
  str << GLOBAL;
  emit_protobj_ref(main, str);
  str << endl;
  str << GLOBAL;
  emit_protobj_ref(integer, str);
  str << endl;
  str << GLOBAL;
  emit_protobj_ref(string, str);
  str << endl;
  str << GLOBAL;
  falsebool.code_ref(str);
  str << endl;
  str << GLOBAL;
  truebool.code_ref(str);
  str << endl;
  str << GLOBAL << INTTAG << endl;
  str << GLOBAL << BOOLTAG << endl;
  str << GLOBAL << STRINGTAG << endl;

  //
  // We also need to know the tag of the Int, String, and Bool classes
  // during code generation.
  //
  str << INTTAG << LABEL
      << WORD << intclasstag << endl;
  str << BOOLTAG << LABEL
      << WORD << boolclasstag << endl;
  str << STRINGTAG << LABEL
      << WORD << stringclasstag << endl;
}

//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text()
{
  str << GLOBAL << HEAP_START << endl
      << HEAP_START << LABEL
      << WORD << 0 << endl
      << "\t.text" << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Main"), str);
  str << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Int"), str);
  str << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("String"), str);
  str << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Bool"), str);
  str << endl
      << GLOBAL;
  emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
  str << endl;
}

void CgenClassTable::code_bools(int boolclasstag)
{
  falsebool.code_def(str, boolclasstag);
  truebool.code_def(str, boolclasstag);
}

void CgenClassTable::code_select_gc()
{
  //
  // Generate GC choice constants (pointers to GC functions)
  //
  str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
  str << "_MemMgr_INITIALIZER:" << endl;
  str << WORD << gc_init_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
  str << "_MemMgr_COLLECTOR:" << endl;
  str << WORD << gc_collect_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_TEST" << endl;
  str << "_MemMgr_TEST:" << endl;
  str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}

//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

std::vector<CgenNodeP> CgenClassTable::get_classes()
{
  std::vector<CgenNodeP> classes_;
  for (List<CgenNode> *l = nds; l; l = l->tl())
  {
    CgenNode *node = l->hd();
    classes_.push_back(node);
  }
  return classes_;
}

void CgenClassTable::code_constants()
{
  //
  // Add constants that are required by the code generator.
  //
  stringtable.add_string("");
  inttable.add_string("0");

  stringtable.code_string_table(str, stringclasstag);
  inttable.code_string_table(str, intclasstag);
  code_bools(boolclasstag);
}

void CgenClassTable::code_class_nameTab()
{
  str << CLASSNAMETAB << LABEL;
  std::vector<CgenNodeP> classes_ = get_classes();
  std::reverse(classes_.begin(), classes_.end());
  for (CgenNodeP curr : classes_)
  {
    Symbol curr_name = curr->get_name();
    char *curr_name_str = curr_name->get_string();
    StringEntry *entry = stringtable.lookup_string(curr_name_str);
    str << WORD;
    entry->code_ref(str);
    str << endl;
  }
}

void CgenClassTable::code_class_objTab()
{
  str << CLASSOBJTAB << LABEL;
  std::vector<CgenNodeP> classes_ = get_classes();
  std::reverse(classes_.begin(), classes_.end());
  for (CgenNodeP curr : classes_)
  {
    Symbol curr_name = curr->get_name();
    char *curr_name_str = curr_name->get_string();
    StringEntry *entry = stringtable.lookup_string(curr_name_str);
    str << WORD;
    str << entry << PROTOBJ_SUFFIX << endl;
    str << WORD;
    str << entry << CLASSINIT_SUFFIX << endl;
  }
}

void CgenClassTable::code_dispTab()
{
  std::vector<CgenNodeP> classes_ = get_classes();
  std::reverse(classes_.begin(), classes_.end());
  for (CgenNodeP curr : classes_)
  {
    curr->fill_dispatch_table();
    sym_node[curr->name] = curr;

    emit_disptable_ref(curr->name, str);
    str << LABEL;

    for (auto &pair : curr->dispatch_table)
    {
      str << WORD;
      str << pair.second << "." << pair.first << endl;
    }
  }
}

void CgenNode::fill_dispatch_table()
{
  std::vector<CgenNodeP> ancestory_curr_class;
  ancestory_curr_class.push_back(this);
  CgenNodeP par = get_parentnd();
  while (par != NULL)
  {
    ancestory_curr_class.push_back(par);
    par = par->get_parentnd();
  }
  std::reverse(ancestory_curr_class.begin(), ancestory_curr_class.end());

  for (CgenNodeP curr : ancestory_curr_class)
  {
    Features curfs = curr->features;
    for (int i = curfs->first(); curfs->more(i); i = curfs->next(i))
    {
      Feature feature = curfs->nth(i);
      if (feature->is_method())
      {
        method_class *method = (method_class *)feature;

        bool found = false;
        for (auto &pair : dispatch_table)
        {
          if (pair.first == method->name)
          {
            pair.second = curr->name;
            found = true;
            break;
          }
        }
        if (found)
        {
          continue;
        }
        else
        {
          auto item = std::make_pair(method->name, curr->name);
          dispatch_table.push_back(item);
        }
      }
    }
  }
}



void CgenClassTable::code_protObj()
{
  std::vector<CgenNodeP> classes_ = get_classes();

  std::reverse(classes_.begin(), classes_.end());
  for (CgenNodeP curr : classes_)
  { 
    
    int tag = get_class_tag(curr->name);
    curr->fill_attr_layout();
    int obj_size = DEFAULT_OBJFIELDS + curr->attr_layout.size();
    str << WORD << "-1" << endl;
    emit_protobj_ref(curr->name, str);
    str << LABEL;
    str << WORD << tag << endl;
    str << WORD << obj_size << endl;
    str << WORD;
    emit_disptable_ref(curr->name, str);
    str << endl;
    for (attr_class *curr_attr : curr->attr_layout)
    {
      if (curr_attr->type_decl == Int)
      {
        str << WORD;
        IntEntry *entry = inttable.lookup_string("0");
         entry -> code_ref(str);
        str << endl;
      }
      else if (curr_attr->type_decl == Str)
      {
        str << WORD;
        StringEntry *entry = stringtable.lookup_string("");
        entry->code_ref(str);
        str << endl;
      }
      else if (curr_attr->type_decl == Bool)
      {
        str << WORD;
        falsebool.code_ref(str);
        str << endl;
      }
      else
      {
        str << WORD;
        IntEntry *entry = inttable.lookup_string("0");
        str << '0';
        str << endl;
      }
    }
  }
}

void CgenNode::fill_attr_layout()
{
  std::vector<CgenNodeP> ancestory_curr_class;
  ancestory_curr_class.push_back(this);
  CgenNodeP par = get_parentnd();
  while (par != NULL)
  {
    ancestory_curr_class.push_back(par);
    par = par->get_parentnd();
  }
  std::reverse(ancestory_curr_class.begin(), ancestory_curr_class.end());

  for (CgenNodeP curr : ancestory_curr_class)
  {
    Features curfs = curr->features;
    for (int i = curfs->first(); curfs->more(i); i = curfs->next(i))
    {
      Feature feature = curfs->nth(i);
      if (feature->is_method() == false)
      {
        attr_class *attr = (attr_class *)feature;
        attr_layout.push_back(attr);
      }
    }
  }

  // Push them to the symbol table
  variables.enterscope();
  int index = 0;
  
  for (attr_class* curr : attr_layout){
    std::pair<int, int>* value = new std::pair<int, int>();
    Symbol key = curr -> name;
    value->first = 0;
    value->second = index;
    variables.addid(key, value);
    index++;
  }
}

int CgenClassTable::get_class_tag(Symbol given_name)
{
  int index = -1;
  for (int i = 0; i < int(classes_ordered.size()); ++i)
  {
    if (classes_ordered[i]->name == given_name)
    {
      index = i;
      break;
    }
  }
  return index;
}

void CgenClassTable::fill_class_tag()
{ 
  std::vector<CgenNodeP> classes_ = get_classes();
  std::reverse(classes_.begin(), classes_.end());
  classes_ordered = classes_;
}

void CgenClassTable::code_init(){
  std::vector<CgenNodeP> classes_ = get_classes();
  std::reverse(classes_.begin(), classes_.end());
  for (CgenNodeP curr : classes_){
    emit_init_ref(curr->name, str);
    str << LABEL;
    emit_addiu(SP, SP, -12, str);
    emit_store(FP, 3, SP, str);
    emit_store(SELF, 2, SP, str);
    emit_store(RA, 1, SP, str);
    emit_addiu(FP, SP, 16, str);
    emit_move(SELF, ACC, str);

    CgenNodeP parent = curr->get_parentnd();

    if(parent -> name != No_class){
      str << JAL;
      emit_init_ref(parent->name, str);
      str << endl;
    }

    if(curr -> basic() == 0){
      for (int i = 0; i < int(curr->attr_layout.size()); ++i){

        attr_class* curr_attr = curr->attr_layout[i];
        Expression curr_init = curr_attr -> init;
        if(curr_init -> is_no_expr() == false){
          curr_init -> code(str, curr, this);
          emit_store(ACC, i+3, SELF, str);
        }
      }
    }
    emit_move(ACC, SELF, str);
    emit_load(FP, 3, SP, str);
    emit_load(SELF, 2, SP, str);
    emit_load(RA, 1, SP, str);
    emit_addiu(SP, SP, 12, str);
    str << RET << endl;
  }
}

CgenClassTable::CgenClassTable(Classes classes, ostream &s) : nds(NULL), str(s)
{

  // stringclasstag = get_class_tag(Str) /* Change to your String class tag here */;
  // intclasstag =    get_class_tag(Int) /* Change to your Int class tag here */;
  // boolclasstag =   get_class_tag(Bool) /* Change to your Bool class tag here */;

  enterscope();
  if (cgen_debug)
    cout << "Building CgenClassTable" << endl;
  install_basic_classes();
  install_classes(classes);
  build_inheritance_tree();
  
  fill_class_tag();
  stringclasstag = get_class_tag(Str);
  intclasstag = get_class_tag(Int);
  boolclasstag = get_class_tag(Bool);

  code();
  traverse_tree();
  exitscope();
}

void CgenClassTable::traverse_tree()
{
  std::vector<CgenNodeP> classes_ = get_classes();
  for (CgenNodeP curr : classes_)
  {
    Features curfs = curr->features;
    if (!curr->basic())
    {
      for (int i = curfs->first(); curfs->more(i); i = curfs->next(i))
      {
        Feature feature = curfs->nth(i);
        if (feature->is_method())
        {
          method_class *method = (method_class *)feature;
          str << curr->name << "." << method->name << LABEL;
          method->code(str, curr, this);
        }
      }
    }
  }
}

void CgenClassTable::install_basic_classes()
{

  // The tree package uses these globals to annotate the classes built below.
  // curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

  //
  // A few special class names are installed in the lookup table but not
  // the class list.  Thus, these classes exist, but are not part of the
  // inheritance hierarchy.
  // No_class serves as the parent of Object and the other special classes.
  // SELF_TYPE is the self class; it cannot be redefined or inherited.
  // prim_slot is a class known to the code generator.
  //
  addid(No_class,
        new CgenNode(class_(No_class, No_class, nil_Features(), filename),
                     Basic, this));
  addid(SELF_TYPE,
        new CgenNode(class_(SELF_TYPE, No_class, nil_Features(), filename),
                     Basic, this));
  addid(prim_slot,
        new CgenNode(class_(prim_slot, No_class, nil_Features(), filename),
                     Basic, this));

  //
  // The Object class has no parent class. Its methods are
  //        cool_abort() : Object    aborts the program
  //        type_name() : Str        returns a string representation of class name
  //        copy() : SELF_TYPE       returns a copy of the object
  //
  // There is no need for method bodies in the basic classes---these
  // are already built in to the runtime system.
  //
  install_class(
      new CgenNode(
          class_(Object,
                 No_class,
                 append_Features(
                     append_Features(
                         single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                         single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
                     single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
                 filename),
          Basic, this));

  //
  // The IO class inherits from Object. Its methods are
  //        out_string(Str) : SELF_TYPE          writes a string to the output
  //        out_int(Int) : SELF_TYPE               "    an int    "  "     "
  //        in_string() : Str                    reads a string from the input
  //        in_int() : Int                         "   an int     "  "     "
  //
  install_class(
      new CgenNode(
          class_(IO,
                 Object,
                 append_Features(
                     append_Features(
                         append_Features(
                             single_Features(method(out_string, single_Formals(formal(arg, Str)),
                                                    SELF_TYPE, no_expr())),
                             single_Features(method(out_int, single_Formals(formal(arg, Int)),
                                                    SELF_TYPE, no_expr()))),
                         single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
                     single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
                 filename),
          Basic, this));

  //
  // The Int class has no methods and only a single attribute, the
  // "val" for the integer.
  //
  install_class(
      new CgenNode(
          class_(Int,
                 Object,
                 single_Features(attr(val, prim_slot, no_expr())),
                 filename),
          Basic, this));

  //
  // Bool also has only the "val" slot.
  //
  install_class(
      new CgenNode(
          class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())), filename),
          Basic, this));

  //
  // The class Str has a number of slots and operations:
  //       val                                  ???
  //       str_field                            the string itself
  //       length() : Int                       length of the string
  //       concat(arg: Str) : Str               string concatenation
  //       substr(arg: Int, arg2: Int): Str     substring
  //
  install_class(
      new CgenNode(
          class_(Str,
                 Object,
                 append_Features(
                     append_Features(
                         append_Features(
                             append_Features(
                                 single_Features(attr(val, Int, no_expr())),
                                 single_Features(attr(str_field, prim_slot, no_expr()))),
                             single_Features(method(length, nil_Formals(), Int, no_expr()))),
                         single_Features(method(concat,
                                                single_Formals(formal(arg, Str)),
                                                Str,
                                                no_expr()))),
                     single_Features(method(substr,
                                            append_Formals(single_Formals(formal(arg, Int)),
                                                           single_Formals(formal(arg2, Int))),
                                            Str,
                                            no_expr()))),
                 filename),
          Basic, this));
}

// CgenClassTable::install_class
// CgenClassTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//
void CgenClassTable::install_class(CgenNodeP nd)
{
  Symbol name = nd->get_name();

  if (probe(name))
  {
    return;
  }

  // The class name is legal, so add it to the list of classes
  // and the symbol table.
  nds = new List<CgenNode>(nd, nds);
  addid(name, nd);
}

void CgenClassTable::install_classes(Classes cs)
{
  for (int i = cs->first(); cs->more(i); i = cs->next(i))
    install_class(new CgenNode(cs->nth(i), NotBasic, this));
}

//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
  for (List<CgenNode> *l = nds; l; l = l->tl())
    set_relations(l->hd());
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNodeP nd)
{
  CgenNode *parent_node = probe(nd->get_parent());
  nd->set_parentnd(parent_node);
  parent_node->add_child(nd);
}

void CgenNode::add_child(CgenNodeP n)
{
  children = new List<CgenNode>(n, children);
}

void CgenNode::set_parentnd(CgenNodeP p)
{
  assert(parentnd == NULL);
  assert(p != NULL);
  parentnd = p;
}

void CgenClassTable::code()
{
  
  if (cgen_debug)
    cout << "coding global data" << endl;
  code_global_data();

  if (cgen_debug)
    cout << "choosing gc" << endl;
  code_select_gc();

  if (cgen_debug)
    cout << "coding constants" << endl;
  code_constants();

  //                 Add your code to emit
  //                   - prototype objects
  //                   - class_nameTab
  //                   - dispatch tables
  //

  if (cgen_debug)
    cout << "coding class_nameTab" << endl;
  code_class_nameTab();

  if (cgen_debug)
    cout << "coding class_objTab" << endl;
  code_class_objTab();

  if (cgen_debug)
    cout << "coding dispTab for all classes" << endl;
  code_dispTab();

  if (cgen_debug)
    cout << "coding protObj for all classes" << endl;
  code_protObj();

  if (cgen_debug)
    cout << "coding global text" << endl;
  code_global_text();
  
  if (cgen_debug)
    cout << "coding init for all classes" << endl;
  code_init();

  //                 Add your code to emit
  //                   - object initializer
  //                   - the class methods
  //                   - etc...
}

CgenNodeP CgenClassTable::root()
{
  return probe(Object);
}

///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP ct) : class__class((const class__class &)*nd),
                                                                       parentnd(NULL),
                                                                       children(NULL),
                                                                       basic_status(bstatus)
{
  stringtable.add_string(name->get_string()); // Add class name to string table
}

//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

void method_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{
  // Push foramls to the symbol table
  curr->variables.enterscope();
  int index = 0;
  Formal curr_formal;
  int size = 0;
  
  for (int j = formals->first(); formals->more(j); j = formals->next(j)){
    size++;
  }
  for (int j = formals->first(); formals->more(j); j = formals->next(j)){
    std::pair<int, int>* value = new std::pair<int, int>();
    curr_formal = formals->nth(j);
    Symbol key = curr_formal -> get_name();
    value->first = 1;
    value->second = (size-1) - index;
    curr->variables.addid(key, value);
    curr->varLen = size;
    index++;
  }
    
  // push stack pointer down
  emit_addiu(SP, SP, -12 - (formals->len() * 4), s);
  // push fp
  emit_store(FP, 3, SP, s);
  // push s0
  emit_store("$s0", 2, SP, s);
  // push ra
  emit_store(RA, 1, SP, s);
  // change FP pointer
  emit_addiu(FP, SP, 16, s);
  // set $s0 = ACC
  emit_move("$s0", ACC, s);

  // generate code on expression
  expr->code(s, curr, ct);

  // pop fp, s0, ra
  emit_load(FP, 3, SP, s);
  emit_load("$s0", 2, SP, s);
  emit_load(RA, 1, SP, s);
  // readjust stack pointer and return
  emit_addiu(SP, SP, 12 + (formals->len() * 4), s);
  emit_return(s);

  curr->variables.exitscope();
}

void assign_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct){
  expr -> code(s, curr, ct);
  std::pair<int, int> value = *(curr->variables.lookup(name));
  //variable is an attribute
  if(value.first == 0) {
    int offset = value.second;
    emit_store(ACC, offset+3, SELF, s);
  }
  //variable is a formal
  if(value.first == 1) {
    int offset = value.second;
    emit_store(ACC, offset, FP, s);
  }
  //variable is defined in let
  if(value.first == 2) {
    int offset = value.second;
    emit_store(ACC, offset, FP, s);
  }
}

void static_dispatch_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{
  for (auto i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->code(s, curr, ct);
    emit_push(ACC, s);
  }
  expr->code(s, curr, ct);
  // if obj == void, abort
  emit_bne(ACC, ZERO, label_index, s);
  emit_load_imm(T1, get_line_number(), s);
  emit_jal("_dispatch_abort", s);
  emit_label_def(label_index, s);
  label_index++;
  std::string dispatchTab = type_name->get_string();
  dispatchTab += DISPTAB_SUFFIX;
  emit_load_address(T1, (char *)dispatchTab.c_str(), s);
  std::vector< std::pair<Symbol, Symbol> > disTab = sym_node[type_name]->dispatch_table;
  int offset = 0;
  for (int i = 0; i < int(disTab.size()); i++) {
    std::pair<Symbol, Symbol> pair = disTab[i];
    if (pair.first == name) {
      emit_load(T1, i, T1, s);
      emit_jalr(T1, s);
    }
  }
}

void dispatch_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{
  
  for (auto i = actual->first(); actual->more(i); i = actual->next(i)) {
    actual->nth(i)->code(s, curr, ct);
    emit_push(ACC, s);
  }

  expr->code(s, curr, ct);
  
  // if obj == void, abort
  emit_bne(ACC, ZERO, label_index, s);
  emit_load_imm(T1, get_line_number(), s);
  emit_jal("_dispatch_abort", s);
  emit_label_def(label_index, s);
  label_index++;
  emit_load(T1, 2, ACC, s);
  Symbol class_ = expr->get_type() == SELF_TYPE ? curr->name : expr->get_type();
  std::vector< std::pair<Symbol, Symbol> > disTab = sym_node[class_]->dispatch_table;
  for (int i = 0; i < int(disTab.size()); i++) {
    std::pair<Symbol, Symbol> pair = disTab[i];
    if (pair.first == name) {
      emit_load(T1, i, T1, s);
      emit_jalr(T1, s);
    }
  }
}

void cond_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct){
  pred -> code(s, curr, ct);
  emit_load(T1, 3, ACC, s);
  int if_false = label_index++;
  emit_beqz(T1, if_false, s);
  then_exp -> code(s, curr, ct);
  int end = label_index++;
  emit_branch(end, s);
  s << "label" << if_false << LABEL;
  else_exp -> code(s, curr, ct);
  s << "label" << end << LABEL;
}

void loop_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct){
  int if_true = label_index++;
  s << "label" << if_true << LABEL;
  
  pred -> code(s, curr, ct);
  emit_load(T1, 3, ACC, s);
  
  int end = label_index++;
  emit_beq(T1, ZERO, end, s);

  body -> code(s, curr, ct);
  emit_branch(if_true, s);
  
  s << "label" << end << LABEL;
  emit_move(ACC, ZERO, s);
}

CgenNodeP CgenClassTable::find_class(Symbol class_name){
  std::vector<CgenNodeP> classes_ = get_classes();
  std::reverse(classes_.begin(), classes_.end());
  for (CgenNodeP curr : classes_){
    if(curr -> name == class_name){
      return curr;
    }
  }
}

void typcase_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct){
  expr -> code(s, curr, ct);

  int starting_label_index = label_index;
  emit_bne(ACC, ZERO, ++label_index, s);
  s << LA << ACC << " str_const0" << endl;
  s << LI << T1 << " " << get_line_number() << endl;
  s << JAL << "_case_abort2" << endl;

  std::vector< std::tuple< Case, int, int> > branches;
  std::vector<Case> cases_vector;

  int num_cases = 0;
  for (auto i = cases->first(); cases->more(i); i = cases->next(i)){
    Case curr_case = cases->nth(i);
    
    cases_vector.push_back(curr_case);
    num_cases++;
  }
  curr->variables.enterscope();

  for(int i = 0; i<num_cases; i++){

    int max_class_tag = -1;
    Case max_case;

    int index = 0;

    for (Case curr_case : cases_vector){

      std::pair<int, int>* value = new std::pair<int, int>();
      Symbol key = curr_case -> get_name();
      value->first = 2;
      value->second = index;
      curr->variables.addid(key, value);
      index++;

      int curr_class_tag = ct -> get_class_tag(curr_case->get_type());
      if(curr_class_tag > max_class_tag){
        max_class_tag = curr_class_tag;
        max_case = curr_case;
      }
    }

    auto it = std::find(cases_vector.begin(), cases_vector.end(), max_case);
      if (it != cases_vector.end()) {
        cases_vector.erase(it);
      }

    Symbol max_case_type = max_case -> get_type();
    CgenNodeP curr_type_node = ct -> find_class(max_case_type);

    List<CgenNode> *children = curr_type_node -> get_children();
    int max_tag_child = max_class_tag;

    CgenNodeP max_child;

    while(children != NULL){
    for (List<CgenNode> *l = children; l; l = l->tl()){
      CgenNodeP child = l->hd();
      int tag_child = ct -> get_class_tag(child -> name);
      if(tag_child > max_tag_child){
        max_tag_child = tag_child;
        max_child = child;
      } 
    }

    children = max_child -> get_children();
    }


    std::tuple< Case, int, int> item;
    item = std::make_tuple(max_case, max_class_tag, max_tag_child);
    branches.push_back(item);
  }
  label_index++;
  int j = 1;
  int top_label_index = starting_label_index + 1;
  for (auto &my_tuple : branches){
    s << "label" << top_label_index << LABEL;
    if(j ==  1){
      emit_load(T2, 0, ACC, s);
    }
    emit_blti(T2, std::get<1>(my_tuple), label_index, s);
    emit_bgti(T2, std::get<2>(my_tuple), label_index, s);
    
    top_label_index = label_index;
    
    label_index++;
    j++;
    Expression expr = std::get<0>(my_tuple) -> get_expression();
    expr -> code(s, curr, ct);
    s << BRANCH << "label" << starting_label_index << endl;  
  }
  s << "label" << top_label_index << LABEL;
  s << JAL << "_case_abort" << endl;
  s << "label" << starting_label_index << LABEL;

  curr->variables.exitscope();

}

void block_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  for (int i = body->first(); body->more(i); i = body->next(i)) {
    body->nth(i)->code(s, curr, ct);
  }
}

void let_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  emit_store("$s1", 1, FP, s);
  if (init->type != nullptr) {
    init->code(s, curr, ct);
  } else if (type_decl == Str) {
    emit_load_string(ACC, stringtable.lookup_string(""), s);
  } else if (type_decl == Int) {
    emit_load_int(ACC, inttable.lookup_string("0"), s);
  } else if (type_decl == Bool) {
    emit_load_bool(ACC, falsebool, s);
  } else {
    emit_move(ACC, ZERO, s);
  }
  curr->variables.enterscope();
  std::pair<int, int>* value = new std::pair<int, int>();
  Symbol key = identifier;
  value->first = 2;
  value->second = curr->varLen;
  curr->varLen++;
  curr->variables.addid(key, value);
  emit_push(ACC, s);
  body->code(s, curr, ct);
  curr->varLen--;
  curr->variables.exitscope();
  emit_load("$s1", 1, FP, s);
}

void plus_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  emit_store("$s1", 1, FP, s);
  e1->code(s, curr, ct);
  emit_move("$s1", ACC, s);
  e2->code(s, curr, ct);
  emit_jal("Object.copy", s);
  emit_load(T2, 3, ACC, s);
  emit_load(T1, 3, "$s1", s);
  emit_add(T1, T1, T2, s);
  emit_store(T1, 3, ACC, s);
  emit_load("$s1", 1, FP, s);
}

void sub_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  emit_store("$s1", 1, FP, s);
  e1->code(s, curr, ct);
  emit_move("$s1", ACC, s);
  e2->code(s, curr, ct);
  emit_jal("Object.copy", s);
  emit_load(T2, 3, ACC, s);
  emit_load(T1, 3, "$s1", s);
  emit_sub(T1, T1, T2, s);
  emit_store(T1, 3, ACC, s);
  emit_load("$s1", 1, FP, s);
}

void mul_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{
  emit_store("$s1", 1, FP, s);
  e1->code(s, curr, ct);
  emit_move("$s1", ACC, s);
  e2->code(s, curr, ct);
  emit_jal("Object.copy", s);
  emit_load(T2, 3, ACC, s);
  emit_load(T1, 3, "$s1", s);
  emit_mul(T1, T1, T2, s);
  emit_store(T1, 3, ACC, s);
  emit_load("$s1", 1, FP, s);
}

void divide_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{
  emit_store("$s1", 1, FP, s);
  e1->code(s, curr, ct);
  emit_move("$s1", ACC, s);
  e2->code(s, curr, ct);
  emit_jal("Object.copy", s);
  emit_load(T2, 3, ACC, s);
  emit_load(T1, 3, "$s1", s);
  emit_div(T1, T1, T2, s);
  emit_store(T1, 3, ACC, s);
  emit_load("$s1", 1, FP, s);
}

void neg_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  e1->code(s, curr, ct);
  emit_jal("Object.copy", s);
  emit_load(T1, 3, ACC, s);
  emit_neg(T1, T1, s);
  emit_store(T1, 3, ACC, s);
}

void lt_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  emit_store("$s1", 1, FP, s);
  e1->code(s, curr, ct);
  emit_move("$s1", ACC, s);
  e2->code(s, curr, ct);
  emit_load(T1, 3, "$s1", s);
  emit_load(T2, 3, ACC, s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_blt(T1, T2, label_index, s);
  emit_load_bool(ACC, BoolConst(0), s);
  emit_label_def(label_index, s);
  label_index++;
  emit_load("$s1", 1, FP, s);
}

void eq_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  emit_store("$s1", 1, FP, s);
  e1->code(s, curr, ct);
  emit_move("$s1", ACC, s);
  e2->code(s, curr, ct);
  emit_move(T1, "$s1", s);
  emit_move(T2, ACC, s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_beq(T1, T2, label_index, s);
  emit_load_bool(ACC, BoolConst(0), s);
  emit_jal("equality_test", s);
  emit_label_def(label_index, s);
  label_index++;
  emit_load("$s1", 1, FP, s);
}

void leq_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  emit_store("$s1", 1, FP, s);
  e1->code(s, curr, ct);
  emit_move("$s1", ACC, s);
  e2->code(s, curr, ct);
  emit_load(T1, 3, "$s1", s);
  emit_load(T2, 3, ACC, s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_bleq(T1, T2, label_index, s);
  emit_load_bool(ACC, BoolConst(0), s);
  emit_label_def(label_index, s);
  label_index++;
  emit_load("$s1", 1, FP, s);
}

void comp_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{
  int prev = label_index;
  int end = label_index++;
  e1->code(s, curr, ct);
  emit_load(T1, 3, ACC, s);
  emit_load_bool(ACC, BoolConst(0), s);
  emit_beqz(T1, prev, s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_label_def(end, s);
}

void int_const_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{
  //
  // Need to be sure we have an IntEntry *, not an arbitrary Symbol
  //
  emit_load_int(ACC, inttable.lookup_string(token->get_string()), s);
}

void string_const_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{
  emit_load_string(ACC, stringtable.lookup_string(token->get_string()), s);
}

void bool_const_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  emit_load_bool(ACC, BoolConst(val), s);
}

void new__class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct){
  if(type_name != SELF_TYPE){
    s << LA << ACC << " " << type_name << PROTOBJ_SUFFIX << endl;
    emit_jal("Object.copy", s);
    s << JAL << type_name  << CLASSINIT_SUFFIX << endl;
  }
  else{
    s << LA << T1 << "class_objTab" <<endl;
    emit_load(T2, 0, SELF, s);
    s << SLL << T2 << " " << T2 << " "<< "3" << endl;
    s << ADDU << T1 << " " << T1 << " " << T2 << endl;
    emit_push(T1, s);
    emit_load(ACC, 0, T1, s);
    emit_jal("Object.copy", s);
    emit_load(T1, 1, SP, s);
    emit_addiu(SP, SP, 4, s);
    s << JALR << T1 << endl;
  }
}

void isvoid_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  e1->code(s, curr, ct);
  emit_load(T1, 3, ACC, s);
  emit_load_bool(ACC, BoolConst(0), s);
  emit_bne(T1, ZERO, label_index, s);
  emit_load_bool(ACC, BoolConst(1), s);
  emit_label_def(label_index, s);
  label_index++;
}

void no_expr_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct)
{ 
  emit_move(ACC, ZERO, s);
}

void object_class::code(ostream &s, CgenNodeP curr, CgenClassTable* ct){
  if (name == self) {
    emit_move(ACC, SELF, s);
  }
  else{
    std::pair<int, int> value = *(curr->variables.lookup(name));
    //variable is an attribute
    if(value.first == 0) {
      int offset = value.second;
      emit_load(ACC, offset+3, SELF, s);
    }
    //variable is a formal
    if(value.first == 1) {
      int offset = value.second;
      emit_load(ACC, offset, FP, s);
    }
    //variable is defined in let or case
    if(value.first == 2) {
      int offset = value.second;
      emit_load(ACC, offset, FP, s);
    }
  }
}

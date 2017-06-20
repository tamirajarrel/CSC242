KB file format:
Symbols:
#list of symbols, which are unique string identifiers
Compounds:
#list of Compounds, format:
#<id> <lhs> <op> <rhs> <inKB>
#id:   Unique Identifier (different from any Symbol)
#lhs:  left side (null if unary) (name of Symbol or compound--compound must already exist)
#op:   operator (null if postive unary)
#inKB: boolean which is false if this is just an expression used to build a larger sentence and should not be in the KB
Target:
#id of one Symbol to check entailment
#If we're in Inference/ this must be of the Compound form but leave off the inKB part

# Operator Specification:
# not = not
# and = and
# or = or
# imp = implies
# bicond = biconditional

## Remove phase 2

## Proposal

 * Make line-slicing a blank token. (just ignore it)
 * Handle line-slicing inside literal string, line comments and comments
 * Warning about line-slicing in the middle of any other token other than 
   literal string, line comments and comments
 * warning about line-slicing inside line-comments - deprecated
 * warning about line-slicing inside text-line  deprecated (preprocessor phase)
   (any preprocessor line with # it is normal to find line-slicing)


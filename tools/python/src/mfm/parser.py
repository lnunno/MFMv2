#!/usr/bin/env python3
'''

Requires:

python3
pyparsing

Created on Oct 22, 2014

@author: lnunno
'''
from pyparsing import Keyword, delimitedList, Word, alphas, alphanums,\
    nestedExpr

def main():
    reg     = Keyword('RegisterElement',caseless=True)
    elem_id = Word(alphas) + '-' + Word(alphanums)
    reg_expr = reg + nestedExpr(content=elem_id)
    tokens = reg_expr.parseString('RegisterElement(foo-123)')
    pass

if __name__ == '__main__':
    main()
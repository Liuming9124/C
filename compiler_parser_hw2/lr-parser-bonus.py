from parglare import Parser, Grammar
#parglare.__version__ = '0.16.0'

grammar = r"""
E: E '+' E  {left, 1}
 | E '-' E  {left, 1}
 | E '*' E  {left, 2}
 | E '/' E  {left, 2}
 | '(' E ')'
 | number;

terminals
number: /-?\d+/;
"""
def add( x, y):
  print(f'{x} add {y} equals {x+y}')
  return x+y
def min( x, y):
  print(f'{x} cut {y} equals {x-y}')
  return x-y
def mul( x, y):
  n = x*y
  n = int(n)
  print(f'{x} multiply {y} equals {n}')
  return n
def div( x, y):
  n = x/y
  n = int(n)
  print(f'{x} divide {y} equals {n}')
  return n

actions = {
    "E": [lambda _, n: add(n[0],n[2]),
          lambda _, n: min(n[0],n[2]),
          lambda _, n: mul(n[0],n[2]),
          lambda _, n: div(n[0],n[2]),
          lambda _, n: n[1],
          lambda _, n: n[0]],
    "number": lambda _, value: int(value),
}

g = Grammar.from_string(grammar)
parser = Parser(g, debug=False, actions=actions)


path = './test.txt'
f = open(path,'r')

cnt=0
for line in f.readlines():
    cnt=cnt+1
    result = parser.parse(line)
    print()
    print(f'Answer{cnt} => {result}')
    print()
    print('==========')

f.close()


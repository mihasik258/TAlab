from lark import Lark, Transformer, Token

from pprint import pprint

robot_grammar = r"""
start: statement*

?statement: var_decl
          | procedure_def
          | procedure_call
          | assign
          | inc_stmt
          | dec_stmt
          | if_stmt
          | while_stmt
          | step_stmt
          | left_stmt
          | right_stmt
          | back_stmt
          | map_decl
          | map_stmt

var_decl: "VAR" NAME ":" type "=" expr
map_decl: "MAP" NAME                         -> map_decl


assign: NAME ":=" expr

inc_stmt: "INC" NAME      -> inc
dec_stmt: "DEC" NAME      -> dec

if_stmt: "IF" expr "THEN" statement_block ["ELSE" statement_block] "FI"
statement_block: statement+

while_stmt: "WHILE" expr "DO" statement* "OD"

procedure_def: "PROC" NAME "[" NAME (WS_INLINE NAME)* "]" stmt_block
procedure_call: NAME "[" NAME (WS_INLINE NAME)* "]"
stmt_block: "{" statement* "}"



look_expr: "LOOK"          
step_stmt: "STEP"
left_stmt: "LEFT"
right_stmt: "RIGHT"
back_stmt: "BACK" 

         
map_stmt: MAP_OP "(" expr expr expr expr ")" -> map_call

?expr: expr "or" and_expr       -> or_
     | and_expr

?and_expr: and_expr "and" not_expr  -> and_
         | not_expr

?not_expr: "not" not_expr           -> not_
         | comp_expr

?comp_expr: arith_expr "GT" arith_expr     -> gt
          | arith_expr "LT" arith_expr     -> lt
          | arith_expr "EQ" arith_expr     -> eq
          | arith_expr

?arith_expr: arith_expr "+" term      -> add_
           | arith_expr "-" term      -> sub_
           | term

?term: term "*" factor                -> mul
     | term "/" factor                -> div
     | factor

?factor: TRUE              -> true
       | FALSE             -> false
       | NAME              -> var
       | SIGNED_NUMBER     -> number
       | "(" expr ")"      -> paren_expr
       | "LOOK"            -> look_expr

TRUE: "true"
FALSE: "false"

type: "INT"     -> int_type
    | "CINT"     -> cint_type
    | "BOOLEAN"    -> bool_type
    | "CBOOLEAN"    -> cbool_type
    
MAP_OP.2: "BAR" | "EMP" | "SET" | "CLR"

%import common.CNAME -> NAME
%import common.SIGNED_NUMBER
%import common.WS
%import common.WS_INLINE
%ignore WS
"""


# Трансформер для преобразования синтаксического дерева
class RobotTransformer(Transformer):
    def start(self, items):
        return {"type": "start", "body": items}

    def var_decl(self, items):
        name, type_node, value = items
        return {
            "type": "var_decl",
            "name": str(name),
            "var_type": type_node,
            "value": value
        }

    def add_(self, items):
        return {'type': 'add', 'left': items[0], 'right': items[1]}

    def sub_(self, items):
        return {'type': 'sub', 'left': items[0], 'right': items[1]}

    def const_decl(self, items):
        name, type_node, value = items
        return {
            "type": "const_decl",
            "name": str(name),
            "const_type": type_node,
            "value": value
        }

    def assign(self, items):
        return {
            "type": "assign",
            "target": str(items[0]),
            "value": items[1]
        }

    def true(self, _):
        return {'type': 'bool', 'value': True}

    def false(self, _):
        return {'type': 'bool', 'value': False}

    def inc(self, items):
        return {"type": "inc", "target": str(items[0])}

    def dec(self, items):
        return {"type": "dec", "target": str(items[0])}

    def gt(self, items):
        return {'type': 'gt', 'left': items[0], 'right': items[1]}

    def lt(self, items):
        return {'type': 'lt', 'left': items[0], 'right': items[1]}
    def eq(self, items):
        return {'type': 'eq', 'left': items[0], 'right': items[1]}

    def statement_block(self, items):
        return items  # просто список узлов

    def if_stmt(self, items):
        result = {
            'type': 'if_stmt',
            'condition': items[0],
            'then': items[1]
        }
        if len(items) == 3:
            result['else'] = items[2]
        return result

    def step_stmt(self, _):
        return {"type": "step_stmt"}

    def left_stmt(self, _):
        return {"type": "left_stmt"}

    def right_stmt(self, _):
        return {"type": "right_stmt"}

    def back_stmt(self, _):
        return {"type": "back_stmt"}

    def look_expr(self, _):
        return {"type": "look"}



    def int_type(self, _):
        return {"type": "INT", "const": False}

    def bool_type(self, _):
        return {"type": "BOOL", "const": False}

    def cint_type(self, _):
        return {"type": "INT", "const": True}

    def cbool_type(self, _):
        return {"type": "BOOL", "const": True}

    def var(self, items):
        return {
            "type": "var",
            "name": str(items[0])
        }

    def NAME(self, token):
        return str(token)

    def number(self, items):
        return {'type': 'num', 'value': int(items[0])}

    def or_(self, items):
        return {'type': 'or', 'left': items[0], 'right': items[1]}

    def and_(self, items):
        return {'type': 'and', 'left': items[0], 'right': items[1]}

    def not_(self, items):
        return {'type': 'not', 'value': items[0]}

    def while_stmt(self, items):
        # items[0] — условие, остальное — список операторов в теле
        condition = items[0]
        body = items[1:]
        return {
            "type": "while_stmt",
            "condition": condition,
            "body": body
        }

    def procedure_def(self, items):
        name_token = items[0]
        stmt_block_index = next(
            i for i, item in enumerate(items) if isinstance(item, dict) and item.get('type') == 'stmt_block'
        )
        param_tokens = items[1:stmt_block_index]
        param_names = [
            str(p) for p in param_tokens
            if not (isinstance(p, Token) and p.type == 'WS_INLINE')
        ]
        body = items[stmt_block_index]
        return {
            "type": "procedure_def",
            "name": str(name_token),
            "params": param_names,
            "body": body
        }

    def procedure_call(self, items):
        name_token = items[0]
        arg_names = [
            str(arg) for arg in items[1:]
            if not (isinstance(arg, Token) and arg.type == 'WS_INLINE')
        ]
        return {
            "type": "procedure_call",
            "name": str(name_token),
            "args": [{'type': 'var', 'name': name} for name in arg_names]
        }

    def stmt_block(self, items):
        return {
            "type": "stmt_block",
            "body": items
        }

    def MAP_OP(self, token):
        return str(token)

    def map_decl(self, items):
        return {
            "type": "map_decl",
            "name": str(items[0])
        }

    def map_call(self, items):
        return {
            "type": "map_call",
            "op": items[0],
            "result": items[1],
            "map": items[2],
            "x": items[3],
            "y": items[4]
        }

    def map_stmt(self, items):
        return items[0]


class Interpreter:
    def __init__(self, ast):
        self.ast = ast
        self.global_scope = {}
        self.call_stack = []
        self.functions = {}
        self.robot = {'x': 0, 'y': 0, 'dir': 1}
        self.maps = {}
        self.map_width = 0
        self.map_height = 0
        self.procedures = {}
        self.expected_type_stack = []
        self.load_map_from_file("map.txt")
    def load_map_from_file(self, filename):
        with open(filename, "r") as f:
            lines = [line.strip() for line in f if line.strip()]
        if not lines or len(lines) < 2:
            raise ValueError("Invalid map file format")
        try:
            start_goal = list(map(int, lines[0].split()))
            if len(start_goal) != 4:
                raise ValueError
            sx, sy, gx, gy = start_goal
        except ValueError:
            raise ValueError("First line must contain: start_x start_y goal_x goal_y")
        map_lines = lines[1:]
        self.map = [list(map(int, line.split())) for line in map_lines]

        self.map_height = len(self.map)
        self.map_width = len(self.map[0])
        for row in self.map:
            if len(row) != self.map_width:
                raise ValueError("Map rows must have equal width")

        # Установка позиции робота и цели
        if not (0 <= sx < self.map_width and 0 <= sy < self.map_height):
            raise ValueError("Start position out of bounds")
        if not (0 <= gx < self.map_width and 0 <= gy < self.map_height):
            raise ValueError("Goal position out of bounds")
        self.robot = {"x": sx, "y": sy, "dir": 0}
        self.goal = {"x": gx, "y": gy}
    def scope(self):
        return self.call_stack[-1] if self.call_stack else self.global_scope
    def lookup(self, name):
        for scope in reversed(self.call_stack + [self.global_scope]):
            if name in scope:
                return scope[name]
        raise RuntimeError(f"Undefined variable: {name}")

    def run(self):
        try:
            for node in self.ast['body']:
                self.exec_stmt(node)
        except StopIteration:
            print("DONE")
    def exec_block(self, block_node):
        for stmt in block_node['body']:
            self.exec_stmt(stmt)

    def get_expr_type(self, expr):
        t = expr["type"]
        if t == "num":
            return {"type": "INT", "const": False}
        elif t == "bool":
            return {"type": "BOOL", "const": False}
        elif t == "var":
            return self.current_scope[expr["name"]]["type"]
        elif t == "look":
            return "INT"
        else:
            raise Exception(f"Неизвестный тип выражения: {t}")

    def print_maps(self):
        print("\nMaps:")
        for name, cells in self.maps.items():
            print(f"  {name}:")
            if not cells:
                print(" (empty)")
                continue
            for y in range(self.map_height):
                row = []
                for x in range(self.map_width):
                    val = cells.get((x, y), None)
                    if val is None:
                        row.append("?")
                    else:
                        row.append("1" if val else "0")
                print("    " + "".join(row))
        print()

    def exec_stmt(self, node):
        #print(f"exec_stmt: node = {node} ({type(node).__name__})")
        t = node['type']
        if t == 'var_decl':
            name = node['name']
            value = self.eval_expr(node['value'])
            var_type = node['var_type']
            if name in self.scope():
                raise RuntimeError(f"Variable '{name}' already defined")
            self.scope()[name] = {
                'value': value,
                'is_const': var_type["const"],
                'type': var_type
            }
        elif t == 'assign':
            name = node['target']
            var_type = self.lookup(name)["type"]
            self.expected_type_stack.append(var_type["type"])
            value = self.eval_expr(node['value'])
            self.expected_type_stack.pop()
            var = self.lookup(name)
            if self.lookup(name)["is_const"]: raise RuntimeError(f"Cannot assign to constant '{name}'")
            var['value'] = value
        elif t == 'procedure_def':
            self.procedures[node["name"]] = {"args": node["params"], "body": node["body"]}
        elif t == "procedure_call":
            proc = self.procedures[node["name"]]
            param_names = proc["args"]
            arg_exprs = node["args"]
            if len(arg_exprs) != len(param_names):
                raise Exception(f"Аргументов не совпадает: ожидалось {len(param_names)}")

            outer_scope = self.scope()
            new_scope = {}
            for name, expr in zip(param_names, arg_exprs):
                new_scope[name] = outer_scope[expr["name"]]

            self.call_stack.append(new_scope)
            self.exec_block(proc["body"])
            self.call_stack.pop()
        elif t == 'stmt_block':
            for stmt in node['body']:
                self.exec_stmt(stmt)
        elif t == 'const_decl':
            name = node['name']
            value = self.eval_expr(node['value'])
            self.scope()[name] = {'value': value, 'is_const': True, 'type': node['const_type']}
        elif t == 'inc':
            var = self.lookup(node['target'])
            if var['is_const']: raise RuntimeError("Cannot increment constant")
            var['value'] += 1
        elif t == 'dec':
            var = self.lookup(node['target'])
            if var['is_const']: raise RuntimeError("Cannot decrement constant")
            var['value'] -= 1
        elif t == 'if_stmt':
            cond = self.eval_expr(node['condition'])
            branch = node['then'] if cond else node.get('else')
            if isinstance(branch, list):
                for stmt in branch:
                    self.exec_stmt(stmt)
            elif branch is not None:
                self.exec_stmt(branch)
        elif t == 'while_stmt':
            while self.eval_expr(node['condition']):
                for stmt in node['body']:
                    self.exec_stmt(stmt)
        elif t == 'step_stmt':
            return self.robotMove()
        elif t == 'left_stmt':
            self.robot_turn(-1)
        elif t == 'right_stmt':
            self.robot_turn(+1)
        elif t == 'back_stmt':
            self.robot_turn(+2)
        elif t == 'map_decl':
            name = node['name']
            if name in self.maps:
                raise RuntimeError(f"Map '{name}' already exists")
            self.maps[name] = self.createEmptyMap()
        elif t == 'map_call':
            op = node['op']
            result = node['result']
            map_node = node['map']

            if map_node['type'] != 'var':
                raise RuntimeError("Invalid map node (expected variable)")
            map_name = map_node['name']

            x = self.eval_expr(node['x'])
            y = self.eval_expr(node['y'])

            if map_name not in self.maps:
                raise RuntimeError(f"Map '{map_name}' not found")

            cell = self.maps[map_name]
            key = (x, y)

            if op == 'BAR':
                val = int(cell.get(key) == 1)
                if result["type"] != "var":
                    raise RuntimeError("Result must be variable")
                self.scope()[result["name"]] = {
                    'value': val,
                    'is_const': False,
                    'type': {'type': 'INT', 'const': False}
                }
            elif op == 'EMP':
                val = int(cell.get(key) == 0)
                var = self.lookup(result["name"])
                if var["is_const"]:
                    raise RuntimeError(f"Cannot assign to constant '{result['name']}'")
                var["value"] = val
            elif op == 'SET':
                self.maps[map_name][key] = 1
            elif op == 'CLR':
                self.maps[map_name][key] = 0
            else:
                raise RuntimeError(f"Unknown map op {op}")

    def createEmptyMap(self):
        return {
            (x, y): False
            for y in range(self.map_height)
            for x in range(self.map_width)
        }

    def call_proc(self, node):
        name = node['name']
        args = [self.eval_expr(arg) for arg in node['args']]

        if name not in self.functions:
            raise RuntimeError(f"Undefined procedure: {name}")

        proc = self.functions[name]
        param_names = proc['params']
        if len(param_names) != len(args):
            raise RuntimeError(f"Procedure '{name}' expects {len(param_names)} arguments, got {len(args)}")

        # Новый фрейм вызова
        local_scope = {}
        for param, arg_val in zip(param_names, args):
            local_scope[param] = {'value': arg_val, 'is_const': False, 'type': None}

        self.call_stack.append(local_scope)
        try:
            for stmt in proc['body']:
                self.exec_stmt(stmt)
        finally:
            self.call_stack.pop()


    def _check_bounds(self, x, y):
        if not (0 <= x < self.map_width and 0 <= y < self.map_height):
            raise RuntimeError("Map access out of bounds")

    def checkbounds(self, x, y):
        return not (0 <= x < self.map_width and 0 <= y < self.map_height)
    def look_distance(self):
        x, y = self.robot['x'], self.robot['y']
        dx, dy = self.get_direction()
        distance = 0
        while True:
            x += dx
            y += dy
            if self.checkbounds(x, y) or self.map[y][x] == 1:
                break
            distance += 1
        return distance

    def check_goal_reached(self):
        if self.robot["x"] == self.goal["x"] and self.robot["y"] == self.goal["y"]:
            print("Goal reached at", self.robot["x"], self.robot["y"])
            self.print_maps()
            raise StopIteration
    def robotMove(self):
        dx, dy = self.get_direction()
        nx = self.robot['x'] + dx
        ny = self.robot['y'] + dy
        if 0 <= nx < self.map_width and 0 <= ny < self.map_height and self.map[ny][nx] == 0:
            self.robot['x'] = nx
            self.robot['y'] = ny
            self.check_goal_reached()
            print(self.robot['x'],self.robot['y'])
            return 1
        else:
            return 0

    def robot_turn(self, delta):
        self.robot['dir'] = (self.robot['dir'] + delta) % 4
        #print(self.robot['dir'])

    def get_direction(self):
        return [(0, -1), (1, 0), (0, 1), (-1, 0)][self.robot['dir']]

    def eval_expr(self, node, expected_type=None):
        t = node['type']
        if t == 'num':
            return node['value']
        elif t == "procedure_call":
            self.exec_stmt(node)  # исполняем как обычно
            for param in self.procedures[node["name"]]["args"]:
                entry = self.current_scope[param]
                if expected_type is None or entry["type"] == expected_type:
                    return entry["value"]
            raise RuntimeError(f"No returnable parameter of type {expected_type}")
        elif t == 'bool':
            return node['value']
        elif t == 'var':
            return self.lookup(node['name'])['value']
        elif t == 'add':
            return self.eval_expr(node['left']) + self.eval_expr(node['right'])
        elif t == 'sub':
            return self.eval_expr(node['left']) - self.eval_expr(node['right'])
        elif t == 'gt':
            return self.eval_expr(node['left']) > self.eval_expr(node['right'])
        elif t == 'lt':
            return self.eval_expr(node['left']) < self.eval_expr(node['right'])
        elif t == 'eq':
            return self.eval_expr(node['left']) == self.eval_expr(node['right'])
        elif t == 'and':
            return self.eval_expr(node['left']) and self.eval_expr(node['right'])
        elif t == 'or':
            return self.eval_expr(node['left']) or self.eval_expr(node['right'])
        elif t == 'not':
            return not self.eval_expr(node['value'])
        elif t == 'look':
            return self.look_distance()
        else:
            raise RuntimeError(f"Unknown expression type: {t}")


if __name__ == "__main__":
    # 1) парсер и трансформер
    parser = Lark(robot_grammar, parser="lalr", propagate_positions=True)
    transformer = RobotTransformer()

    # 2) читаем исходник
    with open("test_input.txt", "r") as f:
        program = f.read()

    try:
        # 3) парсим
        tree = parser.parse(program)
        print("Parsed tree:")
        #print(tree.pretty())

        # 4) преобразуем в AST
        ast = transformer.transform(tree)
        print("\nTransformed AST:")
        #pprint(ast, width=120)

        # 5) создаём интерпретатор, передаём ему AST и запускаем
        interpreter = Interpreter(ast)
        interpreter.run()

        # 6) выводим результат
        print("\n=== Execution finished ===")
        # состояние робота
        print("Robot:", interpreter.robot)  # {'x':…, 'y':…, 'dir':…}

        # карта (двумерный список)
        print("\nMap state:")
        for row in interpreter.map:
            print(row)

        # глобальные переменные
        print("\nVariables:")
        for name, var in interpreter.global_scope.items():
            # var — это dict с ключами 'value', 'is_const', 'type'
            print(f"  {name}: {var['value']} (type={var['type']['type']}, const={var['is_const']})")

    except Exception as e:
        print("Error:", e)

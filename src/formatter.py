import re
from pathlib import Path

# 定义词法类别及对应样式
TOKEN_STYLES = {
    'keyword': 'color: blue; font-weight: bold;',
    'identifier': 'color: black;',
    'number': 'color: darkorange;',
    'operator': 'color: darkred;',
    'separator': 'color: teal;',
    'comment': 'color: green; font-style: italic;',
    'unknown': 'color: gray;'
}

KEYWORDS = {"const", "int", "void", "if", "else", "while", "break", "continue", "return"}
OPERATORS = {"+", "-", "*", "/", "%", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "=", "!"}
SEPARATORS = {";", ",", "(", ")", "{", "}", "[", "]"}

# 定义正则
TOKEN_REGEX = [
    ('comment', re.compile(r'//.*|/\*.*?\*/', re.DOTALL)),
    ('number', re.compile(r'\b(0x[\da-fA-F]+|0[0-7]*|\d+)\b')),
    ('identifier', re.compile(r'\b[_a-zA-Z][_a-zA-Z0-9]*\b')),
    ('operator', re.compile(r'==|!=|<=|>=|&&|\|\||[+\-*/%=<>&|!]')),
    ('separator', re.compile(r'[;,(){}\[\]]')),
    ('whitespace', re.compile(r'\s+'))
]


# 分析单行并返回 HTML
def highlight_line(line: str) -> str:
    result = ''
    pos = 0
    while pos < len(line):
        for tok_type, pattern in TOKEN_REGEX:
            match = pattern.match(line, pos)
            if match:
                value = match.group(0)
                if tok_type == 'identifier' and value in KEYWORDS:
                    tok_type = 'keyword'
                elif tok_type == 'whitespace':
                    result += value.replace(' ', '&nbsp;')
                    pos = match.end()
                    break
                style = TOKEN_STYLES.get(tok_type, 'color: gray;')
                result += f'<span style="{style}">{value}</span>'
                pos = match.end()
                break
        else:
            value = line[pos]
            result += f'<span style="{TOKEN_STYLES["unknown"]}">{value}</span>'
            pos += 1
    return result


# 生成 HTML 页面
def generate_html(input_path: Path, output_path: Path):
    lines = input_path.read_text(encoding='utf-8').splitlines()
    with output_path.open('w', encoding='utf-8') as f:
        f.write('<!DOCTYPE html><html><head><meta charset="UTF-8">')
        f.write('<style>body { background: #f8f8f8; font-family: monospace; line-height: 1.4; white-space: pre; }</style></head><body><pre><code>')
        for line in lines:
            f.write('    ' + highlight_line(line) + '\n')
        f.write('</code></pre></body></html>\n')

# 主流程
if __name__ == '__main__':
    input_file = Path('../code/test1.sy')
    output_file = Path('../output/test2.html')
    output_file.parent.mkdir(parents=True, exist_ok=True)
    generate_html(input_file, output_file)
    print(f"已生成高亮 HTML: {output_file}")

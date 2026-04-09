#include <bits/stdc++.h>
using namespace std;

#define EPSILON -1

struct NFAedge{
    int in; //接收到in转移到to
    int to;
};
struct NFAnode{
    vector<NFAedge> next; //后继状态
    int out_number; //终态时的序号
    string out_class; //终态时的类型
    bool finish; //是否为终态
}NFA[10005];
int NFA_cnt = 0;

struct Regular_edge{
    int from, to;
};

unordered_set<int> character_set;

void make_NFAedge(int from, int to, int c) {
    NFA[from].next.push_back({c, to});
    if (c != EPSILON) character_set.insert(c);
}

std::map<char, int> op_priority = {
    {'*', 3},
    {'@', 2}, //表示连接
    {'|', 1},
    {'(', 0}
};

Regular_edge handle_op(char op, Regular_edge edge1, Regular_edge edge2) {
    if (op == '*') {
        int x = ++NFA_cnt, y = ++NFA_cnt;
        make_NFAedge(edge2.to, edge2.from, EPSILON);
        make_NFAedge(x, edge2.from, EPSILON);
        make_NFAedge(edge2.to, y, EPSILON);
        make_NFAedge(x, y, EPSILON);
        return {x, y};
    } else if (op == '|') {
        int x = ++NFA_cnt, y = ++NFA_cnt;
        make_NFAedge(x, edge1.from, EPSILON);
        make_NFAedge(x, edge2.from, EPSILON);
        make_NFAedge(edge1.to, y, EPSILON);
        make_NFAedge(edge2.to, y, EPSILON);
        return {x, y};
    } else if (op == '@') {
        make_NFAedge(edge1.to, edge2.from, EPSILON);
        return {edge1.from, edge2.to};
    }
    return {0, 0};
}

string solve_expression(string expression) { //这里考虑的尚不全面，也不考虑输入不合法的情况
    string res = "";
    bool inBracket = false; //在inBracket中需要额外加一个|
    bool inQuote = false; //在""中全部按字面量处理
    for (int i = 0; i < expression.length(); i++) {
        if (expression[i] == '"') {
            inQuote = !inQuote;
            res += expression[i];
        }
        else if (inQuote) {
            res += expression[i];
        }
        else if (expression[i] == '[') {
            inBracket = true; res += '(';
        }
        else if (expression[i] == ']') {
            inBracket = false;
            res.pop_back(); //去掉最后一个|
            res += ')';
        }
        else if (expression[i] == '-') { //这里认为在[]中
            if (res.back() == '|') res.pop_back();
            // [] 内前一个字符已经按 "x" 形式加入，这里取回 x 再做范围展开
            char from = res[res.length() - 2];
            res.pop_back(); // "
            res.pop_back(); // x
            res.pop_back(); // "

            i++; //取-后面的字符
            res += '(';
            for (char j = from; j <= expression[i]; j++) {
                if (j != from) res += '|';
                res += '"'; res += j; res += '"';
            }
            res += ')';
            res += '|';
        } else if (expression[i] == '+') {
            if (res.back() == '|') res.pop_back();
            if (res.back() == ')') {
                int now = 1;
                int to = res.length() - 1;
                int from = to - 1;
                while (1) {
                    if (res[from] == ')') now++;
                    else if (res[from] == '(') now--;
                    if (now == 0) break;
                    from--;
                }
                for (int j = from; j <= to; j++) {
                    res += res[j];
                }
                res += '*';
            } else {
                res += res.back(); res += '*';
            }
            if (inBracket) res += '|';
        } else if (expression[i] == '|' || expression[i] == '*') {
            res += expression[i];
        } else if (expression[i] != ' ') {
            if (inBracket) {
                res += '"'; res += expression[i]; res += '"';
                res += '|';
            } else {
                res += expression[i];
            }
        }
    }
    return res;
}

void Regular2NFA(string expression, int out_number, string out_class) {
    expression = solve_expression(expression);
    stack<char> op;
    stack<Regular_edge> edge;
    expression = '(' + expression;
    expression += ')';
    bool inQuote = false; // 双引号内的字符全部按字面量处理

    bool end = false; //是否已经处理完输入的正则表达式，主要用于判断是否需要添加隐式连接符

    auto solve_once = [&]() {
        char t = op.top(); op.pop();
        Regular_edge edge2 = edge.top(); edge.pop();
        Regular_edge edge1;
        if (t == '@' || t == '|') {
            edge1 = edge.top(); edge.pop();
        } else {
            edge1 = {0, 0}; //单目运算符
        }
        edge.push(handle_op(t, edge1, edge2));
    };

    for (int i = 0; i < expression.length(); i++) {
        char c = expression[i];

        bool start = false;
        if (c != ')'  && c != '*' && !(c == '"' && inQuote) && !(!inQuote && c == '|')) start = true;

        if (end && start) { //添加隐式连接符
            while (!op.empty() && op_priority['@'] <= op_priority[op.top()]) {
                solve_once();
            }
            op.push('@');
        }

        end = false;
        if (c == '"') {
            inQuote = !inQuote;
            if (!inQuote) {
                end = true;
            }
            continue;
        }

        if (inQuote) {
            int x = ++NFA_cnt, y = ++NFA_cnt;
            make_NFAedge(x, y, c);
            edge.push({x, y});
            end = true;
            continue;
        }

        switch (c) {
            case '*':
                end = true;
            case '|':
                while (!op.empty() && op_priority[c] <= op_priority[op.top()]) {
                    solve_once();
                }
                op.push(c);
                break;

            case '(':
                // 左括号只入栈，不能在这里归约，否则会把前面的连接符提前弹掉
                op.push(c);
                break;

            case ')':
                while (op.top() != '(') {
                    solve_once();
                }
                op.pop();
                end = true;
                break;

            default:
                cerr << "[Error] Unexpected literal '" << c 
                    << "' outside of quotes in expression: " << expression << endl;
                exit(-1); // 或者抛出异常，强制你检查正则写法
        }
    }
    make_NFAedge(0, edge.top().from, EPSILON); //连接到开始起点
    //处理终态节点
    NFA[edge.top().to].finish = true;
    NFA[edge.top().to].out_number = out_number;
    NFA[edge.top().to].out_class = out_class;
}

void make_NFA() {
    //关键字
    Regular2NFA("[iI][nN][tT]", 1, "KW");
    Regular2NFA("[vV][oO][iI][dD]", 2, "KW");
    Regular2NFA("[rR][eE][tT][uU][rR][nN]", 3, "KW");
    Regular2NFA("[cC][oO][nN][sS][tT]", 4, "KW");
    Regular2NFA("[mM][aA][iI][nN]", 5, "KW");
    Regular2NFA("[fF][lL][oO][aA][tT]", 6, "KW");
    Regular2NFA("[iI][fF]", 7, "KW");
    Regular2NFA("[eE][lL][sS][eE]", 8, "KW");

    //运算符
    Regular2NFA("\"+\"", 9, "OP");
    Regular2NFA("\"-\"", 10, "OP");
    Regular2NFA("\"*\"", 11, "OP");
    Regular2NFA("\"/\"", 12, "OP");
    Regular2NFA("\"%\"", 13, "OP");
    Regular2NFA("\"=\"", 14, "OP");
    Regular2NFA("\">\"", 15, "OP");
    Regular2NFA("\"<\"", 16, "OP");
    Regular2NFA("\"==\"", 17, "OP");
    Regular2NFA("\"<=\"", 18, "OP");
    Regular2NFA("\">=\"", 19, "OP");
    Regular2NFA("\"!=\"", 20, "OP");
    Regular2NFA("\"&&\"", 21, "OP");
    Regular2NFA("\"||\"", 22, "OP");

    // 界符 (SE)
    Regular2NFA("\"(\"", 23, "SE");
    Regular2NFA("\")\"", 24, "SE");
    Regular2NFA("\"{\"", 25, "SE");
    Regular2NFA("\"}\"", 26, "SE");
    Regular2NFA("\";\"", 27, "SE");
    Regular2NFA("\",\"", 28, "SE");

    // 标识符：字母或下划线开头，后跟字母、数字或下划线
    Regular2NFA("[a-zA-Z_][a-zA-Z0-9_]*", 29, "IDN");

    // 浮点数：包含一个小数点的数字串
    Regular2NFA("[0-9]+\".\"[0-9]+", 30, "FLOAT");

    // 整数：数字串
    Regular2NFA("[0-9]+", 31, "INT");
}

struct DFAnode{
    map<int, int> next;
    set<int> from_NFA; //包含了NFA的哪些点
    int out_number; //终态时的序号
    string out_class; //终态时的类型
    bool finish; //是否为终态
    int to_final; //当前的DFA在DFA最小化的过程中属于哪一个集合
    DFAnode() {
        out_number = INT_MAX;
        finish = false;
    }
}DFA[10005];

struct smallest_DFA_node{
    map<int, int> next;
    int out_number; //终态时的序号
    string out_class; //终态时的类型
    bool finish; //是否为终态
    smallest_DFA_node() {
        out_number = INT_MAX;
        finish = false;
    }
}final_DFA[10005];

int DFA_cnt;

void make_DFAedge(int from, int to, int c) {
    DFA[from].next[c] = to;
}

void NFA2DFA() {
    map<set<int>, int> visited;
    set<int> DFA_start = {0};
    queue<int> q; q.push(0);
    while (!q.empty()) {
        int now = q.front(); q.pop();
        for (auto x : NFA[now].next) {
            if (x.in == EPSILON && DFA_start.find(x.to) == DFA_start.end()) {
                q.push(x.to); DFA_start.insert(x.to);
            }
        }
    }
    visited[DFA_start] = 0;
    DFA[0].from_NFA = DFA_start;

    set<int> st;
    for (int i = 0; i <= DFA_cnt; i++) {
        for (auto c : character_set) {
            st.clear();
            for (auto From : DFA[i].from_NFA) {
                for (auto To : NFA[From].next) {
                    if (To.in == c) {
                        st.insert(To.to);
                    }
                }
            }
            if (st.empty()) continue;
            for (auto x : st) q.push(x);
            while (!q.empty()) {
                int now = q.front(); q.pop();
                for (auto x : NFA[now].next) {
                    if (x.in == EPSILON && st.find(x.to) == st.end()) {
                        q.push(x.to); st.insert(x.to);
                    }
                }
            }
            if (visited.count(st)) {
                make_DFAedge(i, visited[st], c);
            } else {
                ++DFA_cnt;
                visited[st] = DFA_cnt;
                DFA[DFA_cnt].from_NFA = st;
                make_DFAedge(i, DFA_cnt, c);
            }
        }
    }

    for (int i = 0; i <= DFA_cnt; i++) {
        for (auto x : DFA[i].from_NFA) {
            if (NFA[x].finish) {
                DFA[i].finish = true;
                if (NFA[x].out_number < DFA[i].out_number) {
                    DFA[i].out_number = NFA[x].out_number;
                    DFA[i].out_class = NFA[x].out_class;
                }
            }
        }
    }
}

void minimize_DFA() {
    vector<vector<int>> nodes;
    vector<int> final_state, not_final_state;
    for (int i = 0; i <= DFA_cnt; i++) {
        if (DFA[i].finish) {
            DFA[i].to_final = 1;
            final_state.push_back(i);
        } else {
            DFA[i].to_final = 0;
            not_final_state.push_back(i);
        }
    }
    nodes.push_back(not_final_state);
    nodes.push_back(final_state);
    while (1) {
        bool new_final_DFA = false;
        for (int i = 0; i < nodes.size(); i++) {
            for (auto c : character_set) {
                int to1 = -2, to2 = -2;
                char dif = '#';
                for (auto x : nodes[i]) {
                    if (DFA[x].next.count(c)) {
                        if (to1 == -2) to1 = DFA[DFA[x].next[c]].to_final;
                        else if (to1 != DFA[DFA[x].next[c]].to_final) {
                            to2 = DFA[DFA[x].next[c]].to_final;
                            dif = c;
                            break;
                        }
                    } else {
                        if (to1 == -2) to1 = -1;
                        else if (to1 != -1) {
                            to2 = -1;
                            dif = c;
                            break;
                        }
                    }
                }
                if (to2 != -2) {
                    new_final_DFA = true;
                    vector<int> old_final_DFA_node, new_final_DFA_node;
                    for (auto x : nodes[i]) {
                        int to = -2;
                        if (DFA[x].next.count(c)) to = DFA[DFA[x].next[c]].to_final;
                        else to = -1;

                        if (to == to1) {
                            old_final_DFA_node.push_back(x);
                        } else {
                            new_final_DFA_node.push_back(x);
                            DFA[x].to_final = nodes.size();
                        }
                    }
                    nodes[i] = old_final_DFA_node;
                    nodes.push_back(new_final_DFA_node);
                }
            }

            //考虑终态状态不一致的
            if (DFA[nodes[i][0]].finish) {
                vector<int> old_final_DFA_node, new_final_DFA_node;
                old_final_DFA_node.push_back(nodes[i][0]);
                for (int j = 1; j < nodes[i].size(); j++) {
                    if (DFA[nodes[i][j]].out_number != DFA[nodes[i][0]].out_number) {
                        new_final_DFA_node.push_back(nodes[i][j]);
                        DFA[nodes[i][j]].to_final = nodes.size();
                    } else old_final_DFA_node.push_back(nodes[i][j]);
                }
                nodes[i] = old_final_DFA_node;
                if (!new_final_DFA_node.empty()) {
                    new_final_DFA = true;
                    nodes.push_back(new_final_DFA_node);
                }
            }
        }


        if (!new_final_DFA) {
            break;
        }
    }
    for (int i = 0; i < nodes.size(); i++) {
        int now = nodes[i][0];
        final_DFA[i].finish = DFA[now].finish;
        final_DFA[i].out_class = DFA[now].out_class;
        final_DFA[i].out_number = DFA[now].out_number;
        for (auto x : nodes[i]) {
            for (auto y : DFA[x].next) {
                final_DFA[i].next[y.first] = DFA[y.second].to_final;
            }
        }
    }
}

void lexical_analyze_file(const string& in_file_name, const string& output_file_name) {
    ifstream fin(in_file_name);
    ofstream fout(output_file_name);
    if (!fin.is_open()) {
        cerr << "[Error] Cannot open file: " << in_file_name << endl;
        return;
    }
    if (!fout.is_open()) {
        cerr << "[Error] Cannot create output file: " << output_file_name << endl;
        return;
    }

    string content((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    size_t i = 0;
    int line = 1, col = 1;

    while (i < content.size()) {
        unsigned char uc = static_cast<unsigned char>(content[i]);
        if (isspace(uc)) {
            if (content[i] == '\n') {
                line++;
                col = 1;
            } else {
                col++;
            }
            i++;
            continue;
        }

        int state = 0;
        int last_accept_state = -1;
        size_t last_accept_pos = i;

        size_t j = i;
        while (j < content.size()) {
            int ch = static_cast<unsigned char>(content[j]);
            auto it = DFA[state].next.find(ch);
            if (it == DFA[state].next.end()) break;

            state = it->second;
            j++;

            if (DFA[state].finish) {
                last_accept_state = state;
                last_accept_pos = j;
            }
        }

        if (last_accept_state == -1) {
            fout << content[i] << "\t<ERROR," << line << "," << col << ">" << endl;
            i++;
            col++;
            continue;
        }

        string lexeme = content.substr(i, last_accept_pos - i);
        if (DFA[last_accept_state].out_number <= 28) {
            fout << lexeme << "\t<" << DFA[last_accept_state].out_class << "," << DFA[last_accept_state].out_number << ">" << endl;
        } else {
            fout << lexeme << "\t<" << DFA[last_accept_state].out_class << "," << lexeme << ">" << endl;
        }

        for (size_t k = i; k < last_accept_pos; k++) {
            if (content[k] == '\n') {
                line++;
                col = 1;
            } else {
                col++;
            }
        }
        i = last_accept_pos;
    }
}

int main() {
    make_NFA();
    NFA2DFA();
    minimize_DFA();

    string in_file_name, out_file_name;
    getline(cin, in_file_name);
    getline(cin, out_file_name);
    lexical_analyze_file(in_file_name, out_file_name);
}
*  experiment1 词法分析
* experiment2 对LL1文法分别使用递归下降文法和预测分析法
* experiment3 构造优先函数和优先分析表识别表达式
* experiment4 构造LR1分析表，对赋值表达式进行语法制导翻译

# 优点
1. experiment2中构造LL1文法的预测分析表时，自动生成非终结符的first集和follow集
2. experiment4中构造文法的LR1分析表，是根据龙书实现的，因此experiment4可以取出构造LR1分析表的部分单独用来生成任意文法的LR1分析表
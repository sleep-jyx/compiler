# 自动构造LL(1)文法的fitst、follow集和预测分析表
C++实现的自动分析LL(1)文法的fitst、follow集和预测分析表。

要求：文法需是LL(1)文法，具体说明见initGrammer()。
本例采用的文法如下：
$$
\begin{aligned}
&(1)E \rightarrow E+T|T \\
&(2)T \rightarrow T*F|F \\  
&(3)F \rightarrow(E)| i\\
\end{aligned}
$$
因为LL(1)不应含有左递归，故消去左递归：
$$
\begin{aligned}
&(1) E \rightarrow TE'    \\      
&(2) E'\rightarrow+TE'|\varepsilon \\
&(3) T \rightarrow FT'   \\
&(4) T' \rightarrow*FT'| \varepsilon \\
&(5) F \rightarrow(E)| i \\
\end{aligned}
$$

$\qquad$为了方便起见，继续拆分：
$$
\begin{aligned}
&(1) E \rightarrow TE'    \\      
&(2) E'\rightarrow+TE' \\
&(3) E' \rightarrow \varepsilon \\
&(4) T \rightarrow FT'   \\
&(5) T' \rightarrow*FT' \\
&(6) T' \rightarrow \varepsilon \\
&(7) F \rightarrow(E) \\
&(8) F \rightarrow i \\
\end{aligned}
$$

构造fitst、follow集参考了虎书，算法如下：

构造预测分析表参考了哈工大mooc，算法如下：


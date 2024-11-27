// hello.c
#include <stdio.h>
int main() {
    printf("\033[31;4mRed Underline Text\033[0m\n");
    printf("this is a common color\n");
    printf("\e[31;40m this is a test\e[0m\n");
/*
\e[F;B;Om

	\e 或 \033 声明了转义序列的开始，
	[ 开始定义颜色。
	'F' 为字体颜色，编号30~37；
	'B' 为背景色，编号40~47。
	'O' 为特殊意义代码
	它们的顺序没有关系。
	m 是标记
	m 后面不用跟空格，是所定义的彩色字和背景

  颜色表
    前景 背景颜色
    -------------------------
    30 40 黑色
    31 41 红色
    32 42 绿色
    33 43 黄色
    34 44 蓝色
    35 45 洋红
    36 46 青色
    37 47 白色

    特别代码意义
    -------------------------
    0 OFF
    1 高亮显示
    4 underline
    5 闪烁
    7 反白显示
    8 不可见



*/
    return 0;
}






package com.mytest.debug;

public class PrintLine {
	public static void main(String[] args) {
		test1();
	}

	public static void printLine() {
		StackTraceElement[] trace = new Throwable().getStackTrace(); // 下标为0的元素是上一行语句的信息, 下标为1的才是调用printLine的地方的信息
		StackTraceElement tmp = trace[1];
		System.out.println(tmp.getClassName() + "." + tmp.getMethodName() + "(" + tmp.getFileName() + ":"
				+ tmp.getLineNumber() + ")");
	}

	public static void test() {
		printLine();
	}

	public static void test1() {
		printLine();
		test();
	}
}

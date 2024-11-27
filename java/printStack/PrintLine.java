package com.mytest.debug;

public class PrintLine {
	public static void main(String[] args) {
		test1();
	}

	public static void printLine() {
		StackTraceElement[] trace = new Throwable().getStackTrace(); // �±�Ϊ0��Ԫ������һ��������Ϣ, �±�Ϊ1�Ĳ��ǵ���printLine�ĵط�����Ϣ
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

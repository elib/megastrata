#pragma once

class AutoMatrixStack
{
public:
	AutoMatrixStack(void);
	~AutoMatrixStack(void);

	void Push();
	void Pop();
	void PopAll();

private:
	int stackdepth;
	static int maxdepth;
	static int max_found_depth;
};
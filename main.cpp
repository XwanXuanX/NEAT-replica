#include "Species.h"

int main()
{
    srand((unsigned int)time(NULL));

	CompatDistParams params(1, 1, 1, 1);

	Genome gene1(2, 1, Node::ActFunc::Linear);
	Genome gene2(gene1);
	Genome gene3(gene2);

	gene1.setFitness(1);
	gene2.setFitness(2);
	gene3.setFitness(3);

	Species spe(gene1);
	spe.AddOrganism(gene2, 1.0, params);
	spe.AddOrganism(gene3, 1.0, params);

	spe.Reproduce(13, 0.3);
	/*
    Genome gene1(3, 1, Node::ActFunc::Linear);
    Genome gene2(3, 1, Node::ActFunc::Linear);

    for(unsigned int i = 0; i < 3; i++)
    {
        gene1.Mutate(0, 0, 0, 100, Node::ActFunc::Linear, 100);
        gene2.Mutate(0, 0, 0, 100, Node::ActFunc::Linear, 100);
    }

    double input[] = {1, 2, 3};
    std::vector<double> output;

    std::cout << "GENE1:" << std::endl;
    gene1.PrintGenotype();
    output = gene1.Propagate(input, 3);
    std::cout << output[0] << std::endl << std::endl;

    std::cout << "GENE2:" << std::endl;
    gene2.PrintGenotype();
    output = gene2.Propagate(input, 3);
    std::cout << output[0] << std::endl << std::endl;

    Genome gene3 = gene1.Crossover(gene2, 1, 1);
    for(unsigned int i = 0; i < 3; i++)
    {
        gene3.Mutate(0, 0, 0, 100, Node::ActFunc::Linear, 100);
    }
    std::cout << "GENE3:" << std::endl;
    gene3.PrintGenotype();
    output = gene3.Propagate(input, 3);
    std::cout << output[0] << std::endl << std::endl;

    std::cout << gene1.CompatDistance(gene3, 1, 1, 1, 0);

    return 0;
	*/
}

/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK MainWindProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = MainWindProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = TEXT("MainWClass");
	wndClass.hIconSm = nullptr;

	RegisterClassEx(&wndClass);

	HWND hwnd = CreateWindowEx(0, TEXT("MainWClass"), TEXT("Genome Visual Debugger"), \
			WS_OVERLAPPEDWINDOW, \
			CW_USEDEFAULT, CW_USEDEFAULT, \
			600, 700, \
			nullptr, nullptr, hInstance, nullptr);

	if (hwnd == nullptr)
	{
		std::cout << "Create Window Error!" << std::endl;
		return -1;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK MainWindProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
    case WM_CREATE:
    {

        HWND button = CreateWindow(TEXT("Button"), TEXT("Button"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  
            35, 10, 120, 60, hwnd, (HMENU)1001, NULL, NULL);
    }
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
    {
		PostQuitMessage(0);
		return 0;
    }
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
*/
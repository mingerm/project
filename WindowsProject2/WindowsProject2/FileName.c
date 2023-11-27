/*

전체 코드 뼈대 작성(윈도우 메인함수, 윈도우 프로시저, 다이얼로그 프로시저)

현재는 코드 수정 과정(최적화) 진행 중입니다.


WinMain()
{
	윈도우 구조체 설정 및 등록 : WNDclassEx() 멤버 설정 및
								 RegisterClassEx()으로 등록

	윈도우 생성/ 출력 : HWND hWnd로 윈도우 생성 스타일 및 핸들 설정
						ShowWindow는 메모리에 생성하는 것
						UpdateWindow는 스크린에 출력하는 것

	메세지 루프(반복? -> 위 두 개는 처음만 하고 이후부턴 이것만 해서 그런 듯)
					  : GetMassage()함수로 WndProc에 전달

}
WinProc()
{
	메세지 처리 과정
	switch(message)
	{
	case WM_...:
		break;
	case WM_...:
		break

	default:
		DefWndProc(); (항상 디폴트로 들어가 있어야 함)
	}

}
**
	메세지 루프와 WndProc()의 관계
 GetMssage(&msg,NULL,0,0)
 {
	TranslateMessage(&msg);
	DispatchMessge(&msg);-> 메세지를 WndProc에 전달-> WndProc에서 메세지 수행 후 반환
 }
 GetMessage()-> DispatchMessage()-> WnProc() -> CALLBACK(?)


 WndProc() /DlgProc() : WndProc()에서 DlgProc()를 호출하여 기능 구현

*/

//메인 윈도우 : 숭실대학교 제휴산업 현황이 출력되는 윈도우
//다이얼로그1 : 단과대학 선택 윈도우
//다이얼로그2 : 제휴산업 정보가 표시되는 윈도우(아직 정보 출력 기능 X)

//수정 필요 부분: ****'메인 윈도우 메뉴바'에서 프로그램 정보(갱신 일자 등) 출력 기능 = 해결(메세지박스로 출력)****, 
// '다이얼로그1'에서 확인 버튼 누를 시 프로그램이 종료되는 기능,  전체적인 윈도우 생성(출력) 위치 수정... ++

#include <Windows.h>
#include <wchar.h> //유니코드 쓸 때 필요
#include "Resource.h" //추가하거나 수정한 리소스 가져올 때 필요 (ID가 저장되어 있음) -> 리소스 파일 ProgrammingProjectexample2.rc 에서 확인 가능 / Resource.h에서 각 개체 ID 확인 가능
//Resource.h와 ProgrammingProjectexample2.rc 동시에 파일 열기 불가

//함수 선언 (프로토타입)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); //윈도우 프로시저 함수
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); //다이얼로그 처리 함수
INT_PTR CALLBACK DlgProc2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); //다이얼로그 처리 함수
//WinMain()함수 설정 (인자 4개 필요){윈도우 클래스 등록/윈도우 생성 및 출력/ 메세지 루프(반복?)}

//전역변수 선언

HINSTANCE hInst;//인스턴스 핸들
HWND hWnd;//메인 윈도우 핸들
HWND g_hModelessWnd; //단과 대학 선택 윈도우 핸들
HWND childWindow;// 버튼 클릭 시 나오는 윈도우 핸들
HWND versionWnd;//메뉴에서 정보를 누를 때 나오는 윈도우 핸들


//메인 함수
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nCmdShow
)
{
	//1. 윈도우 클래스 (윈도우 구조체) 생성 [메인 윈도우]
	WNDCLASSEX wcex; //WNDCLASSEX는 좀 더 발전된 윈도우 클래스임 pure C에서도 사용가능
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;//윈도우 이동 또는 늘리고 줄이기 가능
	wcex.lpfnWndProc = WndProc; //함수 포인터 -> 함수 자체를 가리킴
	wcex.cbClsExtra = 0;//거의 사용 X -> 대부분 0으로 초기화
	wcex.cbWndExtra = 0;//거의 사용 X -> 대부분 0으로 초기화
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));//매크로 함수 리소스 아이디를 정수로 변환하여 사용(핸들)
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //흰 바탕
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); //메뉴1 리소스 사용
	wcex.lpszClassName = L"Basic"; //클래스 이름: 아래 윈도우 생성 시(CreateWindow) 동일하기만 하면 큰 상관 X
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON2)); //거의 정형화 됨

	//2. 윈도우 클래스 (윈도우 구조체) 등록
	RegisterClassEx(&wcex);

	//3. 윈도우 생성
	hWnd = CreateWindow(L"Basic", L"SSU",//구조체명, 타이틀바(윈도우 창 명)
		WS_OVERLAPPEDWINDOW,//윈도우 형태
		CW_USEDEFAULT,//출력 X 좌표 윈도우가 나타나는 위치
		CW_USEDEFAULT,//출력 Y 좌표 윈도우가 나타나는 위치
		400, 300, NULL, NULL,//윈도우 너비, 높이, 부모 윈도우, 메뉴바 핸들
		hInstance, NULL);//인스턴스, 여분(NULL)

	//4. 윈도우 출력(메모리 및 스크린)
	ShowWindow(hWnd, nCmdShow); //WM_PAINT와 관련됨 
	UpdateWindow(hWnd);

	//5. 메세지 루프
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))//정형화 되어있는 형식
	{
		TranslateMessage(&msg);//문자 키 또는 입력에 대한 메세지 반환
		DispatchMessage(&msg); //WndProc()함수 호출 종료까지 대기 WndProc()로 메세지 전달 
	}
	return 0; //Winmain()가 종료될 때 메세지 루프가 같이 종료됨 
}
//윈도우 프로시저 (전체 윈도우 함수 메세지 처리)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)//메세지에 따라 case 실행
	{
	case WM_CREATE: //프로그램 실행 시 바로 다이얼로그가 나타남
		g_hModelessWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc); //다이얼로그 프로시저 함수 실행 (resource 안 IDD_DIALOG1을 가져옴) 
		if (g_hModelessWnd == NULL) //다이얼로그 생성 도중 문제 발생 시
		{
			MessageBox(hWnd, L"Dialog creation failed!", L"Error", MB_OK | MB_ICONERROR);
			return -1;
		}
		break;
	case WM_COMMAND:// 메뉴 선택(프로그램 실행 시 메인 윈도우 상단 바 바로 아래 줄) [파일] -> [정보],[종료]
	{
		wmId = LOWORD(wParam);
		switch (wmId) // 수정 필요
		{
		case IDOK:
			DestroyWindow(hWnd);
			break;
		case ID_32771:// **수정 필요 언제 갱신된 정보인지 나오면 좋을 듯함** 메인 윈도우 메뉴 바[파일]-> [정보]
			MessageBox(hWnd, L"최근 수정일: 2023 11 20", L"Version", MB_OK | MB_ICONINFORMATION);
			break;
		case ID_32772: //메인 윈도우 메뉴 바[파일]-> [종료]
			g_hModelessWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc); //다이얼로그 프로시저 함수 실행 (resource 안 IDD_DIALOG1을 가져옴) 
			if (g_hModelessWnd == NULL) //다이얼로그 생성 도중 문제 발생 시
			{
				MessageBox(hWnd, L"Dialog creation failed!", L"Error", MB_OK | MB_ICONERROR);
				return -1;
			}
			break;
		case ID_32773:
			DestroyWindow(g_hModelessWnd); //다이얼로그 종료
			DestroyWindow(hWnd); //윈도우 종료 ==  프로그램 종료
			break;

		default: //남은 작업을 종료 
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}

	case WM_PAINT: //윈도우에 그래픽 출력 
		hdc = BeginPaint(hWnd, &ps);
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		DrawText(hdc, L"숭실대학교 제휴사업 현황", -1, &clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY: //끝낸다는 메세지 프로시저에 전달
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(g_hModelessWnd);
		DestroyWindow(hWnd);
		break;
	default: //남은 작업을 종료 
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//다이얼로그 프로시저 (버튼 클릭 시 생성되는 윈도우 관련 함수 따로 선언, 정의 필요 (ex: ChildDlgProc) / 수정 필요)
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam); //기본적으로 들어가야 함
	HDC hdc; //dc핸들 그래픽 작업 소요 시 필요한 핸들
	PAINTSTRUCT ps;//WM_PAINT에 필요한 구조체

	switch (message)//case에 따라 실행 
	{

	case WM_CLOSE:
		DestroyWindow(hDlg);//다이얼로그 종료
		return (INT_PTR)TRUE;
	case WM_PAINT://필요?
		hdc = BeginPaint(hDlg, &ps);
		EndPaint(hDlg, &ps);
		return (INT_PTR)TRUE;
	case IDOK: //확인 버튼 = IDOK (수정 필요?)
		EndDialog(hDlg, 0);
		PostQuitMessage(0);
		return (INT_PTR)TRUE;

		/*case WM_INITDIALOG:
			return (INT_PTR)TRUE;*/ //자동 생성 함수 내부에 있던 코드 주석 처리함(사용할 경우 주석 처리 해제)

	case WM_COMMAND: //명령 case 분류
		switch (LOWORD(wParam))//기본적으로 들어가야 함
		{
		case IDC_BUTTON1: //공과대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
			return (INT_PTR)TRUE;
		case IDC_BUTTON2:  //IT대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
		case IDC_BUTTON3: //자연과학대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
		case IDC_BUTTON4: //융특자전대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
		case IDC_BUTTON5: //반도체대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
			return (INT_PTR)TRUE;
		case IDC_BUTTON6: //베어드 교양대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2);
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
			return (INT_PTR)TRUE;
		case IDC_BUTTON7: //인문대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
			return (INT_PTR)TRUE;
		case IDC_BUTTON8: //법과대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
			return (INT_PTR)TRUE;
		case IDC_BUTTON9: //사회과학대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
			return (INT_PTR)TRUE;
		case IDC_BUTTON10: //경제통상대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
			return (INT_PTR)TRUE;
		case IDC_BUTTON11: //경영대학 버튼 ID
			childWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), childWindow, DlgProc2); //DlgProc 실행 생성되는 다이얼로그의 핸들 childWindow에 저장
			if (childWindow != NULL) {
				// 현재 화면 크기 및 작업 영역 크기 가져오기
				RECT desktopRect, workAreaRect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
				GetWindowRect(GetDesktopWindow(), &desktopRect);

				// 다이얼로그 크기 가져오기
				RECT dialogRect;
				GetWindowRect(childWindow, &dialogRect);
				int dialogWidth = dialogRect.right - dialogRect.left;
				int dialogHeight = dialogRect.bottom - dialogRect.top;

				// 화면 중앙에 위치 계산
				int centerX = (desktopRect.right - dialogWidth) / 2;
				int centerY = (desktopRect.bottom - dialogHeight) / 2;

				// 다이얼로그를 화면 중앙으로 이동
				SetWindowPos(childWindow, NULL, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				// 다이얼로그를 보여줌
				ShowWindow(childWindow, SW_SHOWNORMAL);
			}
			else {
				MessageBox(hWnd, L"Dialog creation failed", L"Error", MB_OK | MB_ICONERROR);
			}
			return (INT_PTR)TRUE;
		case IDOK://확인 버튼
			EndDialog(hDlg, IDOK); //다이얼로그 종료
			break;
		}break;
	default: FALSE;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DlgProc2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)// 버튼 눌렀을 때 생성되는 다이얼로그 윈도우 프로시저
{
	HDC hdc;
	PAINTSTRUCT ps;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

	case IDOK: //확인 버튼 = IDOK (수정 필요?)
		DestroyWindow(hDlg);
		DestroyWindow(g_hModelessWnd);
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		return (INT_PTR)TRUE;
		return (INT_PTR)TRUE;
	case WM_CLOSE:
		DestroyWindow(hDlg);//다이얼로그 종료
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

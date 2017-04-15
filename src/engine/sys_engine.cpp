#include "IEngine.h"
#include "IGame.h"

#include "sys.h"

class CEngine final : public IEngine
{
public:
	CEngine() = default;
	~CEngine() = default;

	bool Load( bool dedicated, char* basedir, char* cmdline ) override;
	void Unload() override;

	void SetState( int iState ) override;
	int GetState() override;

	void SetSubState( int iSubState ) override;
	int GetSubState() override;

	int Frame() override;
	double GetFrameTime() override;
	double GetCurTime() override;

	void TrapKey_Event( int key, bool down ) override;
	void TrapMouse_Event( int buttons, bool down ) override;
	void StartTrapMode() override;
	bool IsTrapping() override;
	bool CheckDoneTrapping( int& buttons, int& key ) override;

	int GetQuitting() override;
	void SetQuitting( int quittype ) override;

private:
	int m_nQuitting = 0;
	int m_nDLLState = 0;
	int m_nSubState = 0;

	double m_fCurTime = 0;
	double m_fFrameTime = 0;
	double m_fOldTime = 0;

	bool m_bTrapMode = false;
	bool m_bDoneTrapping = false;
	int m_nTrapKey = 0;
	int m_nTrapButtons = 0;
};

namespace
{
static CEngine g_Engine;
}

IEngine* eng = &g_Engine;

bool CEngine::Load( bool dedicated, char* basedir, char* cmdline )
{
	SetState( 1 );

	bool bInitialized = Sys_InitGame( cmdline, basedir, game->GetMainWindow(), dedicated );

	if( bInitialized )
	{
		//TODO: implement - Solokiller
		//ForceReloadProfile();
	}

	return bInitialized;
}

void CEngine::Unload()
{
	Sys_ShutdownGame();
	//TODO: define constants - Solokiller
	m_nDLLState = 0;
}

void CEngine::SetState( int iState )
{
	m_nDLLState = iState;
	//TODO: implement - Solokiller
	//GameSetState( iState );
}

int CEngine::GetState()
{
	return m_nDLLState;
}

void CEngine::SetSubState( int iSubState )
{
	if( !iSubState != 1 )
	{
		//TODO: implement - Solokiller
		//GameSetSubState( iSubState );
	}
}

int CEngine::GetSubState()
{
	return m_nSubState;
}

int CEngine::Frame()
{
	//TODO: implement - Solokiller
	return 0;
}

double CEngine::GetFrameTime()
{
	return m_fFrameTime;
}

double CEngine::GetCurTime()
{
	return m_fCurTime;
}

void CEngine::TrapKey_Event( int key, bool down )
{
	bool bIsDown = down;

	bool bHandled = false;

	if( m_bTrapMode )
	{
		bIsDown = false;

		if( down )
		{
			m_bTrapMode = false;
			m_bDoneTrapping = true;
			m_nTrapKey = key;
			m_nTrapButtons = 0;

			bHandled = true;
		}
	}

	if( !bHandled )
	{
		//TODO: implement - Solokiller
		//Key_Event( key, bIsDown );
	}
}

void CEngine::TrapMouse_Event( int buttons, bool down )
{
	if( m_bTrapMode && buttons && !down )
	{
		m_bTrapMode = false;
		m_bDoneTrapping = true;
		m_nTrapKey = 0;
		m_nTrapButtons = buttons;
	}
	else
	{
		//TODO: implement - Solokiller
		//ClientDLL_MouseEvent();
	}
}

void CEngine::StartTrapMode()
{
	if( !m_bTrapMode )
	{
		m_bDoneTrapping = false;
		m_bTrapMode = true;
	}
}

bool CEngine::IsTrapping()
{
	return m_bTrapMode;
}

bool CEngine::CheckDoneTrapping( int& buttons, int& key )
{
	if( m_bTrapMode )
	{
		return false;
	}

	if( m_bDoneTrapping )
	{
		key = m_nTrapKey;
		buttons = m_nTrapButtons;
		m_bDoneTrapping = false;
		return true;
	}

	return false;
}

int CEngine::GetQuitting()
{
	return m_nQuitting;
}

void CEngine::SetQuitting( int quittype )
{
	m_nQuitting = quittype;
}

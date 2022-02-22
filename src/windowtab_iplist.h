//*****************************************************************************
//#include	"windowtab_iplist.h"

#ifndef	_WINDOWTAB_IP_LIST_H_
#define	_WINDOWTAB_IP_LIST_H_

#ifndef _DISCOVERY_LIB_H_
	#include	"discovery_lib.h"
#endif // _DISCOVERY_LIB_H_

#ifndef _WINDOW_TAB_H_
	#include	"windowtab.h"
#endif // _WINDOW_TAB_H

//*****************************************************************************
enum
{
	kIPaddrList_Title	=	0,

	kIPaddrList_DiscoveryThrdStatus,
	kIPaddrList_DiscoveryThrdStop,
	kIPaddrList_DiscoveryThrdReStart,
	kIPaddrList_DiscoveryClear,

	kIPaddrList_ErrorMsg,

	kIPaddrList_ClmTitle1,
	kIPaddrList_ClmTitle2,
	kIPaddrList_ClmTitle3,
	kIPaddrList_ClmTitle4,
	kIPaddrList_ClmTitle5,
	kIPaddrList_ClmTitle6,
	kIPaddrList_Outline,

	kIPaddrList_AlpacaDev_01,
	kIPaddrList_AlpacaDev_02,
	kIPaddrList_AlpacaDev_03,
	kIPaddrList_AlpacaDev_04,
	kIPaddrList_AlpacaDev_05,
	kIPaddrList_AlpacaDev_06,
	kIPaddrList_AlpacaDev_07,
	kIPaddrList_AlpacaDev_08,
	kIPaddrList_AlpacaDev_09,
	kIPaddrList_AlpacaDev_10,
	kIPaddrList_AlpacaDev_11,
	kIPaddrList_AlpacaDev_12,
	kIPaddrList_AlpacaDev_13,
	kIPaddrList_AlpacaDev_14,
	kIPaddrList_AlpacaDev_15,
	kIPaddrList_AlpacaDev_16,
	kIPaddrList_AlpacaDev_17,
	kIPaddrList_AlpacaDev_18,
	kIPaddrList_AlpacaDev_19,
	kIPaddrList_AlpacaDev_20,
	kIPaddrList_AlpacaDev_21,
	kIPaddrList_AlpacaDev_22,
	kIPaddrList_AlpacaDev_23,
	kIPaddrList_AlpacaDev_24,
	kIPaddrList_AlpacaDev_25,
	kIPaddrList_AlpacaDev_26,
	kIPaddrList_AlpacaDev_27,
	kIPaddrList_AlpacaDev_28,
	kIPaddrList_AlpacaDev_29,
	kIPaddrList_AlpacaDev_30,
	kIPaddrList_AlpacaDev_31,
	kIPaddrList_AlpacaDev_32,
	kIPaddrList_AlpacaDev_33,
	kIPaddrList_AlpacaDev_34,
//	kIPaddrList_AlpacaDev_35,
//	kIPaddrList_AlpacaDev_36,
//	kIPaddrList_AlpacaDev_37,
//	kIPaddrList_AlpacaDev_38,
//	kIPaddrList_AlpacaDev_39,

	kIPaddrList_AlpacaDev_Last	=	kIPaddrList_AlpacaDev_34,


	kIPaddrList_AlpacaDev_Total,


	kIPaddrList_AlpacaLogo,


	kIPaddrList_last
};


//**************************************************************************************
class WindowTabIPList: public WindowTab
{
	public:
		//
		// Construction
		//
				WindowTabIPList(const int	xSize,
								const int	ySize,
								cv::Scalar	backGrndColor,
								const char	*windowName);
		virtual	~WindowTabIPList(void);

		virtual	void	SetupWindowControls(void);
		virtual	void	ProcessButtonClick(const int buttonIdx);
		virtual	void	ProcessDoubleClick(const int buttonIdx);
		virtual void	ProcessMouseWheelMoved(	const int	widgetIdx,
												const int	event,
												const int	xxx,
												const int	yyy,
												const int	wheelMovement,
												const int	flags);

				void	UpdateIPaddrList(void);
				void	ClearIPaddrList(void);

				int				cPrevAlpacaDevCnt;
				int				cSortColumn;
};




#endif // _WINDOWTAB_IP_LIST_H_

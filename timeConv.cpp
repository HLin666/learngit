/*
功能：获得当前电脑时间（以秒为单位），并将其转换为格里高利历法时间、TAI、GPST、BDT、DOY、JD、MJD。（参考rtklib代码）
原理：Unix时间戳规定了从1970年1月1日 00:00:00 开始计算
      UTC与TAI起点为1958年1月1日0时0分0秒
      格里高利历法时间即年月日时分秒。
	  原子时，取TAI=UTC+37秒。
	  GPST比TAI慢19s，即GPST=TAI-19s，包括GPS周与周内秒。GPS时起点是1980年1月6日0h0m0s
	  BDT,起始历元为2006年1月1日协调世界时（UTC）00时00分00秒，采用周和周内秒计数。北斗时和GPS时相差14秒。BDT=GPST+14s。
	  儒略日JD起点是-4712年1月1日12时。
	  修正儒略日：MJD=JD-2400000.5。
	  年积日DOY：每年1月1日起累计的天数。
*/
#include<stdio.h>;
#include<time.h>;

/*
* 函数time2epoch
* 功能：将BJ_sec转化为格里高利历法时间即BJT_time&date。
* 原理：1.获取系统时间，以秒为单位（Unix时间戳规定了从1970年1月1日 00:00:00 开始计算） 
*       2.将系统时间除一天得秒数86400，并；取整，得到天数
*       3.将系统时间减去天数乘86400，剩余的便是一天中的秒数
*       4.要获取一年当中的月份，考虑到闰年问题，定义一个数组mday，存放四年的月份，共48个月，四年有1461天。
*         mon从0开始for循环，每次加一，直到大于等于48. 若天数大于mday中月份的天数 ，则将天数减掉那个月的天数，
*         直到天数小于那个月的天数，break跳出for循环。这时获得mon，即为四年中的第几个月。 
*       5.1970加上天数除1461再乘四，再加上mon除12，得到年份（不用int转化，会自动转，因为定义的时候就是int）。
*       6.mon除12的余数再加一，得到月份（注意加1）。
*       7.for循环中最后减完的天数再加一，得到日。
*       8.一天中的秒数除3600，获得时。 
*       9.8中除3600的余数除60，得分。
*       10.一天得秒数除60得余数，得秒。
* 输入：BJ_sec，&ep[]
* 输出：&ep[年，月，日，时，分，秒]
*/
 void time2epoch(int BJT_sec, double* ep)
{
	const int mday1[]={
	31,28,31,30,31,30,31,31,30,31,30,31,
	31,28,31,30,31,30,31,31,30,31,30,31,
	31,29,31,30,31,30,31,31,30,31,30,31,//闰年 
	31,28,31,30,31,30,31,31,30,31,30,31};
	
	int t=BJT_sec,days,sec,mon,day;
	days=(int)(t/86400);
	sec=(int)(t-days*86400);
	for(day=days%1461,mon=0;mon<48;mon++){
		if(day>=mday1[mon]) day-=mday1[mon]; else break;
	}
	
	ep[0]=1970+days/1461*4+mon/12;
	ep[1]=mon%12+1;
	ep[2]=day+1;
	ep[3]=sec/3600;
	ep[4]=sec%3600/60;
	ep[5]=sec%60;
}
 /*
 * 函数epoch2TAI
 * 功能：将UTC转换为TAI
 * 原理：TAI = UTC + 37s
 * 输入：UTC_sec
 * 输出：TAI
 */
void epoch2TAI(int UTC_sec, int *TAI)
{	
	*TAI = UTC_sec + 37;//原子时，取TAI=UTC+37秒。
}

/*
* 函数TAI2GPST
* 功能：将TAI转化为GPS时
* 原理：调整两时间系统起点差，TAI1958年1月1日0h0m0s，GPST1980年1月6日0h0m0s
* 输入：TAI
* 输出：GPST[周，周内秒]
*/
void TAI2GPST(int TAI, int* GPST)
{
	int GPST_sec = TAI - ((1980 - 1958) * 365 + 5 + 6) * 86400 - 19;//GPS时，GPST=TAI-19s,两者起点相差((1980 - 1958) * 365 + 5)天 * 86400秒，其中5年闰年
	int GPST_Week = GPST_sec / 604800;//GPS周,一周有604800秒
	int GPST_SecinWeek = GPST_sec % 604800;//GPS周内秒
	GPST[0] = GPST_Week;
	GPST[1] = GPST_SecinWeek;
}

/*
* 函数TAI2BDT
* 功能：将TAI转化为BD时
* 原理：调整两时间系统起点差，TAI1958年1月1日0h0m0s，BDT2006年1月1日0h0m0s
* 输入：TAI
* 输出：BDT[周，周内秒]
*/
void TAI2BDT(int TAI, int* BDT)
{
	int BDT_sec = TAI - ((2006 - 1958) * 365 + 12) * 86400 - 19 + 14;//BDT=GPST+14s
	int BDT_Week = BDT_sec / 604800;//BDS周,一周有604800秒
	int BDT_SecinWeek = BDT_sec % 604800;//BDS周内秒
	BDT[0] = BDT_Week;
	BDT[1] = BDT_SecinWeek;
}

/*
* 函数：calJD
* 功能：计算儒略日
* 原理：儒略日JD起点是-4712年1月1日12时。参考函数time2epoch算法。
* 输入：ep,&JD
* 输出：&JD
*/
#define JD2000 2451545.0//2000年1月1日0h0m0s的儒略日为2451544.5天
void calJD(double ep[],double *JD)
{
	const int runYear[] = {
		31,29,31,30,31,30,31,31,30,31,30,31//闰年 
	};
	const int notRunYear[] = {
		31,28,31,30,31,30,31,31,30,31,30,31//非闰年
	};
	const int mday2[] = {
	31,29,31,30,31,30,31,31,30,31,30,31,//闰年
	31,28,31,30,31,30,31,31,30,31,30,31,
	31,28,31,30,31,30,31,31,30,31,30,31, 
	31,28,31,30,31,30,31,31,30,31,30,31 };

	int year = ep[0];
	int mon = ep[1];
	int day = ep[2];
	double JD1=0, JD2, JD3, JD4;

	//1.计算月日的天数JD1
	if (year % 4 == 0)
	{
		for (int i = mon - 2; i >= 0; i--)
		{
			JD1 += runYear[i];
		}
		JD1 += day;
	}
	else
	{
		for (int i = mon - 2; i >= 0; i--)
		{
			JD1 += notRunYear[i];
		}
		JD1 += day;
	}

	//2.计算时分秒的天数JD2
	JD2 = ep[3] / 24 + ep[4] / (24 * 60) + ep[5] / (24 * 60 * 60);

	//3.计算一周期4年的天数JD3
	JD3 = (int)((year - 2000) / 4 * 1461);

	//计算4年一周期剩余的天数JD4（不算月日时分秒）
	int years = (year - 2000) % 4;//4年一周期剩余的年数
	switch (years)
	{
	case 1:JD4 = 366; break;
	case 2:JD4 = 366 + 365; break;
	case 3:JD4 = 366 + 365 + 365; break;
	}

	//5.计算儒略日JD
	*JD = JD2000 + JD1 + JD2 + JD3 + JD4;
}

/*
* 函数：calMJD
* 功能：计算修正儒略日
* 原理：MJD=JD-2400000.5
* 输入：JD,&MJD
* 输出：&MJD
*/
void calMJD(double JD, double* MJD)
{
	*MJD = JD - 2400000.5;
}

/*
* 函数：calDOY
* 功能：计算年积日
* 原理：年积日DOY：每年1月1日起累计的天数。参考函数time2epoch算法。
* 输入：ep，&DOY
* 输出：&DOY
*/
void calDOY(double ep[], int *DOY)
{
	const int runYear[] = {
		31,29,31,30,31,30,31,31,30,31,30,31//闰年 
	};
	const int notRunYear[] = {
		31,28,31,30,31,30,31,31,30,31,30,31//非闰年
	};

	int year = ep[0];
	int mon = ep[1];
	int day = ep[2];
	if (year % 4 == 0)
	{
		for (int i = mon - 2; i >= 0; i--)
		{
			*DOY += runYear[i];
		}
		*DOY += day;
	}
	else
	{
		for (int i = mon - 2; i >= 0; i--)
		{
			*DOY += notRunYear[i];
		}
		*DOY += day;
	}
}


int main()
{
	int currentTime = time(NULL);//获取当前Unix时间戳
	printf("现在的Unix时间戳为：%d秒\n", currentTime);

	int BJT_sec = currentTime + 28800;//获取北京时间
	double ep[6];//年月日时分秒
	time2epoch(BJT_sec, ep);//将BJ_sec转化为格里高利历法时间即BJT_time&date。
	printf("现在的北京时间为：%.0f年%.0f月%.0f日%.0f时%.0f分%.0f秒\n", ep[0], ep[1], ep[2], ep[3], ep[4], ep[5]);

	int UTC_sec = currentTime + ((1970 - 1958) * 365 + 3) * 86400;//获取UTC时,三年闰年
	printf("现在的UTC时为：%d秒\n", UTC_sec);

	int TAI;
	epoch2TAI(UTC_sec, &TAI);//UTC转化为原子时
	printf("现在的原子时为：%d秒\n", TAI);

	int GPST[2]; //GPST[周，周内秒]
	TAI2GPST(TAI,GPST);//TAI转化为GPS时
	printf("现在的GPS时为：%d周%d秒\n", GPST[0], GPST[1]);

	int BDT[2];//BDT[周，周内秒]
	TAI2BDT(TAI, BDT);//TAI转化为BDS时
	printf("现在的BD时为：%d周%d秒\n", BDT[0], BDT[1]);

	double JD;
	calJD(ep, &JD);
	printf("现在的儒略日为：%f天\n", JD);

	double MJD;
	calMJD(JD, &MJD);
	printf("现在的修正儒略日为：%f天\n", MJD);

	int DOY = 0;
	calDOY(ep,&DOY);
	printf("现在的年积日为：%d天\n", DOY);
	return 0;
}
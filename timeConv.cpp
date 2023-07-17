/*
���ܣ���õ�ǰ����ʱ�䣨����Ϊ��λ����������ת��Ϊ�����������ʱ�䡢TAI��GPST��BDT��DOY��JD��MJD�����ο�rtklib���룩
ԭ����Unixʱ����涨�˴�1970��1��1�� 00:00:00 ��ʼ����
      UTC��TAI���Ϊ1958��1��1��0ʱ0��0��
      �����������ʱ�伴������ʱ���롣
	  ԭ��ʱ��ȡTAI=UTC+37�롣
	  GPST��TAI��19s����GPST=TAI-19s������GPS���������롣GPSʱ�����1980��1��6��0h0m0s
	  BDT,��ʼ��ԪΪ2006��1��1��Э������ʱ��UTC��00ʱ00��00�룬�����ܺ����������������ʱ��GPSʱ���14�롣BDT=GPST+14s��
	  ������JD�����-4712��1��1��12ʱ��
	  ���������գ�MJD=JD-2400000.5��
	  �����DOY��ÿ��1��1�����ۼƵ�������
*/
#include<stdio.h>;
#include<time.h>;

/*
* ����time2epoch
* ���ܣ���BJ_secת��Ϊ�����������ʱ�伴BJT_time&date��
* ԭ����1.��ȡϵͳʱ�䣬����Ϊ��λ��Unixʱ����涨�˴�1970��1��1�� 00:00:00 ��ʼ���㣩 
*       2.��ϵͳʱ���һ�������86400������ȡ�����õ�����
*       3.��ϵͳʱ���ȥ������86400��ʣ��ı���һ���е�����
*       4.Ҫ��ȡһ�굱�е��·ݣ����ǵ��������⣬����һ������mday�����������·ݣ���48���£�������1461�졣
*         mon��0��ʼforѭ����ÿ�μ�һ��ֱ�����ڵ���48. ����������mday���·ݵ����� �������������Ǹ��µ�������
*         ֱ������С���Ǹ��µ�������break����forѭ������ʱ���mon����Ϊ�����еĵڼ����¡� 
*       5.1970����������1461�ٳ��ģ��ټ���mon��12���õ���ݣ�����intת�������Զ�ת����Ϊ�����ʱ�����int����
*       6.mon��12�������ټ�һ���õ��·ݣ�ע���1����
*       7.forѭ����������������ټ�һ���õ��ա�
*       8.һ���е�������3600�����ʱ�� 
*       9.8�г�3600��������60���÷֡�
*       10.һ���������60�����������롣
* ���룺BJ_sec��&ep[]
* �����&ep[�꣬�£��գ�ʱ���֣���]
*/
 void time2epoch(int BJT_sec, double* ep)
{
	const int mday1[]={
	31,28,31,30,31,30,31,31,30,31,30,31,
	31,28,31,30,31,30,31,31,30,31,30,31,
	31,29,31,30,31,30,31,31,30,31,30,31,//���� 
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
 * ����epoch2TAI
 * ���ܣ���UTCת��ΪTAI
 * ԭ����TAI = UTC + 37s
 * ���룺UTC_sec
 * �����TAI
 */
void epoch2TAI(int UTC_sec, int *TAI)
{	
	*TAI = UTC_sec + 37;//ԭ��ʱ��ȡTAI=UTC+37�롣
}

/*
* ����TAI2GPST
* ���ܣ���TAIת��ΪGPSʱ
* ԭ����������ʱ��ϵͳ���TAI1958��1��1��0h0m0s��GPST1980��1��6��0h0m0s
* ���룺TAI
* �����GPST[�ܣ�������]
*/
void TAI2GPST(int TAI, int* GPST)
{
	int GPST_sec = TAI - ((1980 - 1958) * 365 + 5 + 6) * 86400 - 19;//GPSʱ��GPST=TAI-19s,����������((1980 - 1958) * 365 + 5)�� * 86400�룬����5������
	int GPST_Week = GPST_sec / 604800;//GPS��,һ����604800��
	int GPST_SecinWeek = GPST_sec % 604800;//GPS������
	GPST[0] = GPST_Week;
	GPST[1] = GPST_SecinWeek;
}

/*
* ����TAI2BDT
* ���ܣ���TAIת��ΪBDʱ
* ԭ����������ʱ��ϵͳ���TAI1958��1��1��0h0m0s��BDT2006��1��1��0h0m0s
* ���룺TAI
* �����BDT[�ܣ�������]
*/
void TAI2BDT(int TAI, int* BDT)
{
	int BDT_sec = TAI - ((2006 - 1958) * 365 + 12) * 86400 - 19 + 14;//BDT=GPST+14s
	int BDT_Week = BDT_sec / 604800;//BDS��,һ����604800��
	int BDT_SecinWeek = BDT_sec % 604800;//BDS������
	BDT[0] = BDT_Week;
	BDT[1] = BDT_SecinWeek;
}

/*
* ������calJD
* ���ܣ�����������
* ԭ����������JD�����-4712��1��1��12ʱ���ο�����time2epoch�㷨��
* ���룺ep,&JD
* �����&JD
*/
#define JD2000 2451545.0//2000��1��1��0h0m0s��������Ϊ2451544.5��
void calJD(double ep[],double *JD)
{
	const int runYear[] = {
		31,29,31,30,31,30,31,31,30,31,30,31//���� 
	};
	const int notRunYear[] = {
		31,28,31,30,31,30,31,31,30,31,30,31//������
	};
	const int mday2[] = {
	31,29,31,30,31,30,31,31,30,31,30,31,//����
	31,28,31,30,31,30,31,31,30,31,30,31,
	31,28,31,30,31,30,31,31,30,31,30,31, 
	31,28,31,30,31,30,31,31,30,31,30,31 };

	int year = ep[0];
	int mon = ep[1];
	int day = ep[2];
	double JD1=0, JD2, JD3, JD4;

	//1.�������յ�����JD1
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

	//2.����ʱ���������JD2
	JD2 = ep[3] / 24 + ep[4] / (24 * 60) + ep[5] / (24 * 60 * 60);

	//3.����һ����4�������JD3
	JD3 = (int)((year - 2000) / 4 * 1461);

	//����4��һ����ʣ�������JD4����������ʱ���룩
	int years = (year - 2000) % 4;//4��һ����ʣ�������
	switch (years)
	{
	case 1:JD4 = 366; break;
	case 2:JD4 = 366 + 365; break;
	case 3:JD4 = 366 + 365 + 365; break;
	}

	//5.����������JD
	*JD = JD2000 + JD1 + JD2 + JD3 + JD4;
}

/*
* ������calMJD
* ���ܣ���������������
* ԭ����MJD=JD-2400000.5
* ���룺JD,&MJD
* �����&MJD
*/
void calMJD(double JD, double* MJD)
{
	*MJD = JD - 2400000.5;
}

/*
* ������calDOY
* ���ܣ����������
* ԭ���������DOY��ÿ��1��1�����ۼƵ��������ο�����time2epoch�㷨��
* ���룺ep��&DOY
* �����&DOY
*/
void calDOY(double ep[], int *DOY)
{
	const int runYear[] = {
		31,29,31,30,31,30,31,31,30,31,30,31//���� 
	};
	const int notRunYear[] = {
		31,28,31,30,31,30,31,31,30,31,30,31//������
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
	int currentTime = time(NULL);//��ȡ��ǰUnixʱ���
	printf("���ڵ�Unixʱ���Ϊ��%d��\n", currentTime);

	int BJT_sec = currentTime + 28800;//��ȡ����ʱ��
	double ep[6];//������ʱ����
	time2epoch(BJT_sec, ep);//��BJ_secת��Ϊ�����������ʱ�伴BJT_time&date��
	printf("���ڵı���ʱ��Ϊ��%.0f��%.0f��%.0f��%.0fʱ%.0f��%.0f��\n", ep[0], ep[1], ep[2], ep[3], ep[4], ep[5]);

	int UTC_sec = currentTime + ((1970 - 1958) * 365 + 3) * 86400;//��ȡUTCʱ,��������
	printf("���ڵ�UTCʱΪ��%d��\n", UTC_sec);

	int TAI;
	epoch2TAI(UTC_sec, &TAI);//UTCת��Ϊԭ��ʱ
	printf("���ڵ�ԭ��ʱΪ��%d��\n", TAI);

	int GPST[2]; //GPST[�ܣ�������]
	TAI2GPST(TAI,GPST);//TAIת��ΪGPSʱ
	printf("���ڵ�GPSʱΪ��%d��%d��\n", GPST[0], GPST[1]);

	int BDT[2];//BDT[�ܣ�������]
	TAI2BDT(TAI, BDT);//TAIת��ΪBDSʱ
	printf("���ڵ�BDʱΪ��%d��%d��\n", BDT[0], BDT[1]);

	double JD;
	calJD(ep, &JD);
	printf("���ڵ�������Ϊ��%f��\n", JD);

	double MJD;
	calMJD(JD, &MJD);
	printf("���ڵ�����������Ϊ��%f��\n", MJD);

	int DOY = 0;
	calDOY(ep,&DOY);
	printf("���ڵ������Ϊ��%d��\n", DOY);
	return 0;
}
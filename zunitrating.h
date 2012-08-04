#ifndef _ZUNITRATING_H_
#define _ZUNITRATING_H_

enum unit_cross_reference
{
	UCR_WILL_DIE, UCR_EVEN, UCR_WILL_KILL
};

class ZUnitRating
{
public:
	ZUnitRating();

	void Init();
	void InitMallocUCR();
	void InitPopulateUCR();

	bool IsUnit(unsigned char ot, unsigned char oid);
	void InsertCrossReference(unsigned char a_ot, unsigned char a_oid, unsigned char v_ot, unsigned char v_oid, int ucr);
	int CrossReference(unsigned char a_ot, unsigned char a_oid, unsigned char v_ot, unsigned char v_oid);

private:
	char ****unit_cr;
};

#endif

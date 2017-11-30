#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITSIZE 8

typedef unsigned int word;

word arrayToInt(word *array);
word* intToArray(word val);
word getOppositeNumber(word val);
word madd(word lhs, word rhs);
word msub(word lhs, word rhs);
word mmul(word lhs, word rhs);
word mdiv(word lhs, word rhs);
word mmod(word lhs, word rhs);

int main()
{
    word lhs, rhs;
    printf("Enter two operands:\n");
    scanf("%d%d", &lhs, &rhs);
    printf("sum: %d\ndifference: %d\nproduct: %d\nconsult: %d\nmod: %d\n"
           , madd(lhs, rhs), msub(lhs, rhs), mmul(lhs, rhs), mdiv(lhs, rhs), mmod(lhs, rhs));
}

word arrayToInt(word *array)
{
    word i;
    word answer = 0;
    for (i = 0; i < BITSIZE; i++)
    {
        answer = answer * 10 + array[i];
    }
    return answer;
}

word* intToArray(word val)
{
    word i;
    word *answer = (word*)malloc(sizeof(word) * BITSIZE);
    if (0 == val)
    {
        for (i = BITSIZE; i > 0; i--)
        {
            answer[i - 1] = 0;
        }
        return answer;
    }
    else
    {
        for (i = BITSIZE; val != 0; i--)
        {
            answer[i - 1] = val % 10;
            val /= 10;
        }
        for ( ; i > 0; i--)
        {
            answer[i - 1] = 0;
        }
        return answer;
    }
}

word getOppositeNumber(word val)
{
    word i;
    word one = 1;
    word *valArray = intToArray(val);
    for (i = 0; i < BITSIZE; i++)
    {
        valArray[i] = valArray[i] ^ 1;
    }
    val = arrayToInt(valArray);
    return madd(val, one);
}

word madd(word lhs, word rhs)
{
    word *lArray = intToArray(lhs);
    word *rArray = intToArray(rhs);
    word *answerArray = (word*)malloc(sizeof(word) * BITSIZE);
    word carry = 0;
    word i;
    for (i = BITSIZE; i > 0; i--)
    {
        answerArray[i - 1] = lArray[i - 1] ^ rArray[i - 1] ^ carry;
        carry = (lArray[i - 1] & rArray[i - 1]) | (lArray[i - 1] & carry) | (rArray[i - 1] & carry);
    }
    return arrayToInt(answerArray);
}

word msub(word lhs, word rhs)
{
    rhs = getOppositeNumber(rhs);
    return madd(lhs, rhs);
}

word mmul(word lhs, word rhs)
{
    word answer = 0;
    word one = 1;
    word *rArray = intToArray(rhs);
    if(0 == rArray[0])
    {
        while (rhs != 0)
        {
            answer = madd(answer, lhs);
            rhs = msub(rhs, one);
        }
        return answer;
    }
    else
    {
        rhs = getOppositeNumber(rhs);
        answer = mmul(lhs, rhs);
        return getOppositeNumber(answer);
    }
}

word mdiv(word lhs, word rhs)
{
    word answer = 0;
    word one = 1;
    word *lArray = intToArray(lhs);
    word *rArray = intToArray(rhs);
    if (lArray[0] == rArray[0])
    {
        if (0 == lArray[0])
        {
            while (lhs >= rhs)
            {
                lhs = msub(lhs, rhs);
                answer = madd(answer, one);
            }
            return answer;
        }
        else
        {
            lhs = getOppositeNumber(lhs);
            rhs = getOppositeNumber(rhs);
            return mdiv(lhs, rhs);
        }
    }
    else
    {
        if (0 == lArray[0])
        {
            rhs = getOppositeNumber(rhs);
        }
        else
        {
            lhs = getOppositeNumber(lhs);
        }
        answer = mdiv(lhs, rhs);
        return getOppositeNumber(answer);
    }
}

word mmod(word lhs, word rhs)
{
    word temp = mdiv(lhs, rhs);
    temp = mmul(rhs, temp);
    return msub(lhs, temp);
}

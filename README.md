# Fraudulent Transaction Identification

![Image of Fraud](https://github.com/olivertan1999/fraudulent_transaction_identification/blob/master/Fraud-cash.jpg)

There are around 16 million credit cards on issue in Australia, and the number is over 1 billion worldwide. This is a goldmine for cybercriminals who make unauthorised payment to obtain goods or services (i.e.,credit card fraud). Worldwide losses from card fraud is expected to reach US$31 billion in 2020. Banks and card companies are strongly motivated to develop anti-fraud technologies. They prevented two-thirds of the attempted card fraud in the UK in 2018, but this is a never-ending battle.

This program serves the purpose of **processing credit card and transaction records and identify fraudulent transactions**. List of credit card records and transactions to be processed should be in the following format.

> 3tu2iywy 10000 800<br/> 
ceww0p66 150 100<br/>
v0xyil5t 3000 500<br/>
vb3dtxp0 5000 300<br/>
xnwxw8tl 2000 800<br/>
%%%%%%%%%%<br/>
v5buvljyh0lg vb3dtxp0 2020:04:07:03:50:45 42<br/>
1yuy3noa2uxu xnwxw8tl 2020:04:08:04:16:20 729<br/>
gl3utnnwxf49 xnwxw8tl 2020:04:08:05:39:00 670<br/>
9mopqy3snk3h ceww0p66 2020:04:15:08:06:49 86<br/>
6hjqaydtmrq5 vb3dtxp0 2020:04:15:10:09:50 213<br/>
mlgtqk8oo74e ceww0p66 2020:04:15:13:45:29 95<br/>
u7s604f0u6bz xnwxw8tl 2020:04:22:15:30:43 799<br/>
2siil57yqe5k vb3dtxp0 2020:04:23:17:26:20 573<br/>
vzdg2z09t7zp v0xyil5t 2020:04:29:18:03:00 3489<br/>
n6lcvjyrhvwt 3tu2iywy 2020:04:29:23:07:00 4592<br/>

The input starts with a list of credit card records sorted by `card ID`. There are **at least 1 and at most 100 cards**. Each credit card occupies one line with three fields separated by a single space: unique `card ID` (8 alphanumeric characters; no uppercase letters), `daily limit` (the total amount that can be spent in a day; a positive integer), and `transaction limit` (the amount that can be spent in a transaction; a positive integer). The transaction limit does not exceed the daily limit.

The line “%%%%%%%%%%” indicates the end of credit card records and the start of transactions.

The transactions are sorted by `date` and `time`. Each transaction occupies one line with four fields separated by a single space: unique `transaction ID` (12 alphanumeric characters; no uppercase letters), `card ID` (8 alphanumeric characters; no uppercase letters; must have appeared in the card records), transaction `date` and `time` (with format year:month:day:hour:minute:second, two digits for each component), and `transaction amount` (a positive integer, for simplicity). There is no given limit on the number of transaction lines.

The result will be presented in stages for easy visualization: 
- Stage 1: Reading One Credit Card Record
- Stage 2: Reading All Credit Card Records
- Stage 3: Reading the Transactions
- Stage 4: Checking for Fraudulent Transactions

Sample result using the data provided above:
>=========================Stage 1=========================<br/>
Card ID: 3tu2iywy<br/>
Daily limit: 10000<br/>
Transaction limit: 800<br/>
<br/>
>=========================Stage 2=========================<br/>
Number of credit cards: 5<br/>
Average daily limit: 4030.00<br/>
Card with the largest transaction limit: 3tu2iywy<br/>
<br/>
>=========================Stage 3=========================<br/>
v5buvljyh0lg<br/>
1yuy3noa2uxu<br/>
gl3utnnwxf49<br/>
9mopqy3snk3h<br/>
6hjqaydtmrq5<br/>
mlgtqk8oo74e<br/>
u7s604f0u6bz<br/>
2siil57yqe5k<br/>
vzdg2z09t7zp<br/>
n6lcvjyrhvwt<br/>
<br/>
>=========================Stage 4=========================<br/>
v5buvljyh0lg                IN_BOTH_LIMITS<br/>
1yuy3noa2uxu             IN_BOTH_LIMITS<br/>
gl3utnnwxf49              IN_BOTH_LIMITS<br/>
9mopqy3snk3h           IN_BOTH_LIMITS<br/>
6hjqaydtmrq5              IN_BOTH_LIMITS<br/>
mlgtqk8oo74e             OVER_DAILY_LIMIT<br/>
u7s604f0u6bz             IN_BOTH_LIMITS<br/>
2siil57yqe5k                OVER_TRANS_LIMIT<br/>
vzdg2z09t7zp             OVER_BOTH_LIMITS<br/>
n6lcvjyrhvwt                OVER_TRANS_LIMIT<br/>
<br/>

# UID developer Guide

## Purpose

   1. Solve the problem that users without a BOS account can not use DApps on BOS.
   2. Solve the problem of insufficient CPU for common users on the BOS chain.
   3. Solve the problem that users need to confirm frequently when using DApp on BOS, optimize user experience.
   4. Introduce new users to the BOS chain.

Using the UID system, players can use DApps on BOS without a BOS account, and no need for stake BOS to get CPU, NET and other resources.

## System Structure

UID system mainly consists of three parts: authentication server, UID contract and management system. The usage is as follows:

   1. register in the system.
   2. The DApp connected to the UID system require user authorizations in the form of OAuth2.
   3. The DApp can call the `charge` funcion to charge fee directly from the user's personal assets in the system with the authorization obtained in step2 when needed.

### Authentication Server

The authentication server mainly provides the user authorization function for DApps, the access address is: https://test.uid.red/auth the authentication flow is as follows:

1. The user clicks the "Login" button on the DApp Web site, and the DApp Web site redirects the user to the authentication server, passing the following information in the parameters:
	* client_id: the developer account. The developer account must be registered with the UID contract in advance.
	* redirect_uri: the callback address of the authorization result.
	* scope: list of authorization scopes, including currency andamount, in the following format: "1.0000 BOS ^ 1.0000 EOS" (currently only BOS is supported).
	* expire_in: the expiration time of the authorization, in seconds, optional, defaults to 24 hours.
	* pubkey: trades public keys. When requesting authorization, the front end generates a pair of keys, the public key is passed to the authorization server, and the private key is used for the subsequent deduction of the fee
	* state: the current state of the client, optional, can be specified any value, the authentication server returns the value as it is.
	* language: the language used
		* en
		* cn

2. The user enters the username and password in the web page of the authentication server, and after confirming the authorization information, click the "login" button. After performing the login process, the authentication server jumps to the redirect_uri provided in step 1, and carries the authorization results in the url.
	* success: redirect_uri#error=0&username=xxx&scope=xxx&state=xxx&ulimited=false
	* failure: redirect_url#error=access_denied
		* error: authorization status
			* zero: success
			* others: other error messages
	* username: the authorized user
	* scope: user authorization scope
	* state: the state field carried by the DApp when requesting authorization
	* ulimited: whether the user checks the unlimited quota
		* true: unlimited amount
		* false: limited by the scope when deducting tokens from user's assets

3. After a successful authorization, DApp can use the private key generated in step 1 to call the `charge` function.

### UID contract

UID contract provides DApp management, user management and other functions. The DApp related interfaces are as follows:

#### Developer Registration

```
eosio.token transfer(name from, name to, asset quantity, string memo)
```

Developers are required to register by paying a certain amount of deposit, the minumim deposit is 100.0000 BOS. The amount of deposit affects the user's maximum authorization amount (currently 1/100 of the deposit amount). The format of the memo is: `reg^original_memo`

* reg: command type, developer registration
* original_memo: user-defined memo

#### Update Developer Information

```
devupdate(name account, string devname, string avatar, string url, string brief)
```

Update the developer information. Can only be called by the developer account.

* account: registered developer account
* devname: developer name, which is presented to the user when authorization is requested
* account: registered developer account
* devname: developer name
* avatar: developer avatar, 32 bytes (not used yet)
* url: developer redirect uri, 128 bytes
* brief: developers profile, 128 bytes (not used yet)

#### Developer Recharge Deposit

```
eosio.token transfer(name from, name to, asset quantity, string memo)
```

The developers adds his own deposit, the memo format is: `rc^account^original_memo`.

* rc: command type, recharge deposit
* account: developer account
* original_memo: user-defined memo

#### Developer Withdraw Deposit

```
withdraw(name account, asset quantity, string memo)
```

The developer withdraws the deposit, if the quantity after after extraction is less than the minimum deposit, the withdrawal fails. Can only be called by the developer account.

* account: developer account
* quantity: the quantity of the deposit to be withdrawn
* memo: user-defined memo

#### DApp Verification

```
verifyapp(name contract, uint32_t status, string memo)
```

The developer verifies the DApp. Only the DApp that is verified by the developer can be used normally. Can only be called by the developer to which the DApp was registered.

* contract: DApp contact account
* status: DApp status
	* 0: initial state
	* 1: verified
* memo: user-defined memo

#### Developer Quit

```
devquit(name account, string memo)
```

The developer quits and refunds all the deposit.Can only be called by the developer account.

* account: developer account
* memo: user-defined memo

#### DApp Registration

```
appreg(name contract, name developer, string memo)
```

Register a DApp. After the registration, the DApp must also be verified by the developer before it can be used normally. Can only be called by the DApp contract account.

* contact: DApp contract account
* developer: develper account
* memo: user-defined memo

#### Setting DApp Info

```
appupdate(name contract, name developer, string appname, string owner, string logo_url, string url, string brief, string desc)
```

Update the information of the DApp. Can only be called by the contract account or the developer account before the modification.

* contract: DApp contract account
* appname: DApp name, 32 bytes
* owner: DApp developer name, 64 bytes
* logo_url: DApp logo url, 128 bytes
* url: DApp url, 128 bytes
* brief: introduction to DApp, 64 bytes
* desc: DApp details, 512 bytes

#### Setting DApp Info (multi-language)

```
setappinfo(name contract, string appname, string owner, string logo_url, string url, string brief, string desc, string language)
```

Update the information of the DApp. Can only be called by the contract account or the developer account.

* contract: DApp contract account
* appname: DApp name, 32 bytes
* owner: DApp developer name, 64 bytes
* logo_url: DApp logo url, 128 bytes
* url: DApp url, 128 bytes
* brief: introduction to DApp, 64 bytes
* desc: DApp details, 512 bytes
* language: information language
	* cn
	* en

#### DApp Quit

```
appquit(name contract, string memo)
```

DApp quits. Can only be called by the DApp conract account or the developer account to which the DApp belongs.

* contract: DApp contract account
* memo: user-defined memo

#### Transfer To User

```
eosio.token tranfer(name from, name to, asset quantity, string memo)
```

Tranferring tokens to a user in the UID system, or to a mainnet user. When Transferring tokens to a user in the UID system, the `to` should be `uid`, and the speific user is identified in the memo. The memo format is: `tf^username.uid^original_memo`

* tf: command type, transfer
* username: user name
* original_memo: user-defined memo

#### Charge

```
charge(name username, name contract, asset quantity, string memo, uint32_t expire_time, signature sig)
```

The DApp deducts tokens with the private key authorized by the user. Can only be called by `boshuobinode` with the permission `uid`, the corresponding private key is `5KEk58bZuNvjvvPhnkHeCWf5ZQrvBjkNE9W9CFu17qfrwneHfxp`.

* username: the user to be charged from
* contract: DApp contact account
* quantity: the asset to be charged
* memo: user-defined memo (DApp needs to put username into memo to identify the specific account)
* expire_time: the transaction timeout period, seconds since epoch, within 180 seconds of the current time
* sig: converts all parameters except `sig` to strings, join them with `-`, and then sign with the private key authorized by the user.

### Management System

Mainly provides user management function for ordinary users.

## Storage

### User Table

* code: `uid`
* scope: `uid`
* table name: `users`

| Field Name |Type  |Descripition  |
| --- | --- | --- |
| username | name | primary key, user name |
| nickname | string | nickname, 32 bytes |
| avatar | string | avatar url,128 bytes |
| gender | uint8_t | gender, 0: unknown 1: male 2: female |
| brief | string | introduction 128 bytes |
| pubkey | public_key | user public key |

### User Balance Table

* code: `uid`
* scope: username.uid
* table name: `balance`

| Field Name |Type  |Descripition  |
| --- | --- | --- |
| balance | asset | primary key, balance of each currency |

### Developer Table

* code: `uid`
* scope: `uid`
* table name: `developers`

| Field Name |Type  |Descripition  |
| --- | --- | --- |
| developer | name | primary key, developer account |
| name | string | developer name, 32 bytes |
| avatar | string | developer avatar url,128 bytes |
| url | string | developer redirect uri, 128 bytes |
| brief | string | developer profile, 128 bytes |

#### Developer Balance Table

* code: `uid`
* scope: `developer`
* table name: `devbalance`

| Field Name |Type  |Descripition  |
| --- | --- | --- |
| balance | asset | primary key, developer balance of each currency |

#### DApp Table

* code: `uid`
* scope: `uid`
* table name: `dapps`

| Field Name |Type  |Descripition  |
| --- | --- | --- |
| contract | name | primary key, DApp contract account |
| developer | name | index, developer account of DApp |
| status | uint32_t | DApp validation status, 0: not validated 1: validated |
| appname | string | project name, 32 bytes |
| owner | string | project developer, 64 bytes |
| logo_url | string | project logo,128 bytes |
| url | string | project web site, 128 bytes |
| brief | string | project profile, 64 bytes |
| desc | string | project indirect, 128 bytes |

#### User Authorization Table

* code: `uid`
* scope: username.uid
* table name: `auths`

| Field Name |Type  |Descripition  |
| --- | --- | --- |
| contract | name | primary key, DApp contract account |
| pubkey | public_key | transaction public key |
| expire | time_point_sec | index, authorization expiration time |
| credits | vector {asset, asset} | the authorization information for various tokens. limit: limit amount, when 0 means unlimited, used: used amount |

### Process of Development

1. Register as a developer and then register an DApp.
2. Modify to DApp contract to make it compatible with the UID contract.
	* Modify the `transfer` interface. When transferring to a user in the UID system, transfer to `uid` and indentify the specific user in the memo.
	* When received an transfer from `uid`, get the specific transfer user in the memo.
3. Modify the DApp front end to support the UID system.


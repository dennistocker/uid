# 柚ID开发指南

## 目的

1. 解决没有BOS账户的用户无法使用BOS上DApp的问题。
2. 解决BOS链上普通用户CPU不足的问题。
3. 解决BOS链上使用DApp需要用户频繁确认的问题，优化用户体验。
4. 为BOS链引入新用户。

使用柚ID系统，玩家无需BOS账户，也无需抵押BOS获取CPU、NET等资源，即可使用BOS上的DApp。

## 系统构成

柚ID系统主要有三部分构成：认证服务器、柚ID合约和管理系统。使用流程如下：

1. 用户在系统注册。
2. 接入柚ID系统的DApp以OAuth2的方式获取用户授权。
3. 用户在使用DApp的过程中，如果需要收取费用，DApp可以使用第2步中获取的授权信息，直接从玩家的个人资产中扣除相应费用。

### 认证服务器

认证服务器主要为DApp提供用户授权功能，接入地址为：https://bosuid.com/auth ,认证流程如下：

1. 用户在DApp网站点击“登录”按钮，DApp网站将用户重定向到认证服务器，在参数中传递如下信息：
	* client_id: 需要授权的开发者账户。开发者账户必须事先在柚ID合约中进行注册。
	* redirect_uri: 用户授权后，授权结果的回调地址。
	* scope: 授权范围列表，如有多个使用"^"连接，包含币种、额度等信息，格式如下：“1.0000 BOS^1.0000 UID”
	* expire_in: 授权的过期时间，单位为秒，可省略，默认为1小时，最大为1小时。
	* pubkey: 交易公钥。请求授权时，前端生成一对密钥，公钥传给授权服务器，私钥用作后续扣费时的凭证。
	* state: 客户端的当前状态，可选项，可指定任意值，认证服务器将该值原样返回。
    * langeuage: 使用语言, 默认中文
        * cn
        * en

2. 用户在认证服务器的网页中输入用户名、密码，确认授权信息后，点击“登录”按钮。认证服务器在执行完登录流程后跳转到步骤1中提供的redirect_uri，在url中携带授权结果。
	* 成功: redirect_uri#error=0&username=xxx&scope=xxx&state=xxx&ulimited=false
	* 失败: redirect_url#error=access_denied
		* error: 授权状态
			* 0 表示成功
			* 其它 错误信息
		* username: 表示授权的具体用户
		* scope: 用户授权范围
		* state: DApp请求授权时携带的state字段
		* ulimited: 用户是否勾选无限制额度
			* true 表示无限制，此时从用户资产扣除token时不受scope的限制
			* false 从用户资产扣除token时受scope的限制
3. 授权成功后，DApp可使用步骤1中生成的私钥调用柚ID合约`charge`收取费用。

### 柚ID合约

柚ID合约提供DApp管理、用户管理等功能。DApp相关接口如下:

#### 开发者注册

	eosio.token transfer(name from, name to, asset quantity, string memo)

开发者通过自身账户转账支付一定数量的押金才能进行注册，最小押金为100.0000 BOS，押金数量影响用户最大授权额度（目前为押金数量的1/100）。memo的格式为：`reg^original_memo`

* reg: 命令类型，开发者注册
* original_memo: 自定义memo

#### 更新开发者信息

	devupdate(name account, string devname, string avatar, string url, string brief)

更新开发者信息，该接口只能由开发者账户调用。

* account: 已注册的开发者账户
* devname: 开发者名称，在请求授权时展示给用户
* avatar: 开发者头像，32 bytes (暂未使用)
* url: 开发者请求授权时的回调url，128 bytes
* brief: 开发者简介，128 bytes(暂未使用)

#### 开发者充值押金

	eosio.token transfer(name from, name to, asset quantity, string memo)

开发者增加自身押金，memo格式为：`rc^account^original_memo`

* rc: 命令类型: 充值押金
* account:开发者账户
* original_memo: 自定义memo

#### 开发者提取押金

	withdraw(name account, asset quantity, string memo)

开发者提取押金，如果提取后数量小于最小押金，则会提取失败。目前该接口只能由UID合约账户调用。

* account: 开发者账户
* quantity: 要提取的押金的数量
* memo: 自定义memo

#### 开发者验证DApp

	verifyapp(name contract, uint32_t status, string memo)

开发者验证DApp，只有通过开发者验证的DApp才能正常使用。该接口只能由DApp注册时所属的开发者调用。

* contract: DApp合约账号
* status: DApp状态
	* 0: 初始状态
	* 1: 验证通过
* memo: 自定义memo

#### 开发者退出

	devquit(name account, string memo)

开发者退出，退还全部押金。目前接口只能由UID合约账户调用，如要退出请发送邮件进行申请。

* account: 开发者账户
* memo: 自定义memo

#### DApp注册

	appreg(name contract, name developer, string memo);

注册某一个DApp到某个开发者名下，该接口只能由DApp合约账户调用。DApp注册后，必须经过开发者验证，才能正常使用。

* contract: DApp合约账户
* developer: 开发者账户
* memo: 自定义memo

#### DApp修改信息

	appupdate(name contract, name developer, string appname, string owner, string logo_url, string url, string brief, string desc)

contract为DApp合约账户，可根据contract修改其余信息。该接口只能由contract账户或修改之前的developer账户调用。

* contract: DApp合约账户
* developer: DApp开发者账户
* appname: DApp名称，32 bytes
* owner: DApp开发者名称，64 btyes
* logo_url: DApp logo地址，128 bytes
* url：DApp url， 128 bytes
* brief: DApp简介，64 bytes
* desc: DApp详细介绍，512 bytes

#### 设置DApp信息（多语言）

	setappinfo(name contract, string appname, string owner, string logo_url, string url, string brief, string desc, string language)

设置DApp信息的多语言版本。使用`appupdate`接口设置默认语言的DApp描述信息后，如果需要支持其他语言，使用此接口设置对应语言的描述信息。

* contract: DApp合约账户
* appname：DApp名称，32bytes
* owner: DApp开发者名称，64 btyes
* logo_url: DApp logo地址，128 bytes
* url：DApp url， 128 bytes
* brief: DApp简介，64 bytes
* desc: DApp详细介绍，512 bytes
* language: 信息语言
	* cn: 中文
	* en: 英文

#### DApp退出

	appquit(name contract, string memo)

DApp退出，该接口只能由DApp合约账户或该DApp所属的开发者账户调用。

* contract: DApp合约账户
* memo: 自定义memo

#### 向用户转账

	eosio.token tranfer(name from, name to, asset quantity, string memo)

向柚ID系统中用户转账时和向主网用户转账一样，其中to为柚ID合约账户`uid`，具体用户在memo中标识。memo格式为：`rf^username.uid^original_memo`。

* tf: 命令类型，转账
* username: 用户名称
* original_memo: 自定义memo

**与uid合约交互时，需要在username后加.uid后缀以标识是uid系统子账户**

#### 向用户扣款

	charge(name username, name contract, asset quantity, string memo, uint32_t expire_time, signature sig)

DApp通过用户授权的私钥进行扣款，柚ID合约校验通过后，调用tranfer接口向DApp合约账户进行转账。接口需使用账户`uidfreetouse`的`uid`权限进行调用, 对应的私钥为`5JzUD1J4fpPTm1c7fCRfQHbWGEeKsypwAqEwweU2wm7vRCJzTFj`。

* username: 要扣款的用户名
* contract: DApp合约账户
* quantity: 交易额
* expire_time: 交易超时时间，距1970年1月1日0时的秒数，需在当前时间180s以内。
* memo: transfer接口需要的memo信息（**DApp需要将username放入memo以识别具体账户**）
* sig: 将除sig外其余参数转成字符串，以"-"连接，然后使用交易私钥进行签名。

### 管理系统

主要提供用户管理功能，供普通用户使用。

## 存储结构

### 用户表

* code: `uid`
* scope: `uid`
* 表名：`users`

| 字段名 |类型 |说明 |
|------- |---- |---- |
|username |name |主键，用户名 |
|nickname |string |昵称，32 bytes |
|avatar |string |头像url，128 bytes |
|gender |uint8_t |性别，0：未知 1：男 2：女 |
|brief |string |简介，128 bytes |
|pubkey |public_key |用户公钥 |

### 用户余额表

* code: `uid`
* scope: username.uid
* 表名: `balance`

|字段名 |类型 |说明 |
|------- |---- |---- |
|balance |asset |主键，各币种余额 |

### 开发者表

* code: `uid`
* scope:`uid`
* 表名：`developers`

|字段名 |类型 |说明 |
|------- |---- |---- |
|developer |name |主键，开发者账户 |
|name |string |开发者名称，请求授权时展示，32 bytes |
|avatar |string |开发者头像url，128 bytes |
|url |string |开发者回调url，128 bytes |
|brief |string |开发者简介，128 bytes |

### 开发者余额表

* code: `uid`
* scope: developer
* 表名: `balance`

|字段名 |类型 |说明 |
|------- |---- |---- |
|balance |asset |主键，developer各币种押金余额 |

### DApp表

* code: `uid`
* scope: `uid`
* 表名: `dapps`

|字段名 |类型 |说明 |
|------- |---- |---- |
|contract |name |主键，DApp合约账户 |
|developer |name |索引，DApp所属开发者账户 |
|status |uint32_t |DApp验证状态，0：未验证 1：已验证 |
|appname |string |项目名称，32 bytes |
|owner |string |项目开发方，64 bytes |
|logo_url |string |项目logo，128 bytes |
|url |string |项目网址，128 bytes |
|brief |string |项目简介，64 bytes |
|desc |string |项目详细描述，128 bytes |

### 用户授权表

* code: `uid`
* scope: username.uid
* 表名: `auths`

|字段名 |类型 |说明 |
|------- |---- |---- |
|contract |name |主键，DApp合约账户 |
|pubkey |public_key |交易公钥 |
|expire |time_point_sec |索引，授权过期时间 |
|credits |vector{asset, asset} |各种代币的授权信息， limit：授权额度，为0时表示无限额，used：已用额度 |

## 接入流程

1. 开发者通过转账向柚ID系统注册开发者账户和DApp。
2. 修改DApp合约以支持柚ID系统
	* 修改转账接口，向用户转账时修改为向柚ID合约转账，在memo中标识具体用户
	* 接收转账时，从memo中判断具体转账用户
	* 如有其它与用户交互的接口，与转账接口类似，需要从memo中标识具体用户
3. 修改DApp前端，支持柚ID系统。
4. 目前如需提取押金或退出，请发送邮件至uid@boscore.io进行申请，邮件中请包含以下内容
	* 开发者账户
	* 申请类型：“提取押金”或“退出”
	* 如申请提取押金，需注明要提取的数量。

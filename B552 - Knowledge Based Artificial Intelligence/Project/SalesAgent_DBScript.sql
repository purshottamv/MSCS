USE master ;
GO
DROP DATABASE SalesAgent
GO
CREATE DATABASE SalesAgent
GO

USE [SalesAgent]
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO

/***DROP TABLE [dbo].[PRODUCT]***/
CREATE TABLE [dbo].[PRODUCT]
(
PRODUCT_ID int IDENTITY PRIMARY KEY,
PRDOCUT_TYPE varchar(100),
MODEL_NO varchar(100),
BRAND varchar(100),
DISPLAY_SIZE float,
PROCESSOR_SPEED float,
NO_OF_CORES float,
PROCESSOR_BRAND varchar(100),
RAM_SIZE float,
HDD_SIZE float, 
PRICE float,
BATTERY_LIFE float,
CACHE_SIZE	float,
GRAPHICS_CARD_MEM_SIZE float,
OPERATING_SYSTEM varchar(100),
OPERATING_SYSTEM_RANK float,
WEIGHT_OF_PRODUCT float,
WARRANTY float,
NO_OF_USB_PORTS float
)

INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','1','Acer',15.6,2.3,4,'Intel',4,160,482.7,6,4,1000,'Windows 7 Premium',10,7.4,1,3)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','2','Acer',14,2.5,4,'Intel',4,500,729.99,9,3,1000,'Windows 7 Home Premium',9,8.15,1,2)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','3','Alienware',14,2.4,8,'Intel',6,500,1278,4,3,3000,'Windows 7 Home Premium',9,13.5,1,3)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','4','Dell',14,2.4,4,'Intel',6,500,799.99,7,4,2000,'Windows 7 Home Premium',9,7.5,1,2)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','5','Samsung',15.6,1.5,4,'AMD',4,500,496.75,6,4,1000,'Windows 7 Home Premium',9,5.07,1,3)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','6','Lenovo',15.6,2.3,4,'Intel',4,500,449.99,4,4,0,'Windows 7 Home Premium',9,5.73,1,4)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','7','Sony',14,2.5,8,'Intel',6,640,789.99,7,4,0,'Windows 7 Home Premium',9,5.08,1,4)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','8','Toshiba',15.6,1.33,2,'AMD',2,320,359.99,4.5,1,0,'Windows XP Home',5,7,1,2)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','9','Asus',15.6,2.3,8,'Intel',4,500,517.17,5,1,500,'Windows XP Home Premium',7,9.2,1,3)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','10','HP',11.6,1.65,2,'AMD',4,500,375.99,9.5,1,2,'Windows 7 Home premium',9,4.8,1,3)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','11','Toshiba',15.6,2.3,4,'Intel',4,640,499.99,5.42,3,0,'Windows 7 Home Premium',9,5.3,1,3)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','12','Apple',13.3,2.4,2,'Intel',4,500,1119.94,7,2,380,'Mac OS Lion',11,8.4,1,2)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','13','Apple',15.4,2.2,6,'Intel',4,500,1679.94,7,4,1000,'Mac OS Lion',11,10.2,1,2)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','14','Asus',17.3,2.2,16,'Intel',12,1500,1649.99,5,4,3,'Windows 7 Home Premium',9,9.4,1,7)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','15','Samsung',15.6,2.2,6,'Intel',8,1000,1228.97,6,3,500,'Windows 7 Home Premium',9,5,1,3)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','16','Gigabyte',14.1,2,4,'Intel',4,500,699,5.5,2,0,'Windows 7 Home Premium',9,5.6,1,2)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','17','Alienware',18.4,2.2,12,'Intel',8,1000,2199.95,5.5,4,1500,'Windows 7 Home Premium',9,17,1,2)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','18','Alienware',17,2.93,12,'Intel',6,640,3299.84,5,6,2000,'Windows 7 Home Premium',9,22,1,5)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','19','Asus',15.6,1.65,2,'AMD',4,320,433.99,6,4,0,'Windows 7 Home Premium',9,5.72,1,3)
INSERT INTO [dbo].[PRODUCT] VALUES('Laptop','20','HP',15.6,1.6,2,'AMD',4,500,449.95,7,2,2000,'Windows 7 Home Premium',9,6,1,2)

/** SELECT * FROM [dbo].[PRODUCT] **/

GO

/***DROP TABLE [dbo].[RULES]***/
CREATE TABLE [dbo].[RULES]
(
RULE_ID int IDENTITY PRIMARY KEY,
NON_SELECTED_ATTRIBUTE varchar(100),
SELECTED_ATTRIBUTE varchar(100),
MAGNITUDE bit,
EFFECT bit,
REMARKS varchar(5000)
)

INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','HDD_SIZE',1,1,'Less Use of Virtual Memory, leading to More free HDD space')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','HDD_SIZE',0,0,'More Use of Virtual Memory, leading to Less free HDD space')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','PRICE',1,0,'Price will shoot up, due to High Price of Ram')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','PRICE',0,1,'Price will reduce, due to Low Price of Ram')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','BATTERY_LIFE',1,1,'Less Swapping, contributing to less power usage.')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','BATTERY_LIFE',0,0,'More Swapping, contributing to more power usage.')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','CACHE_SIZE',1,1,'Cache miss, could be compensated by Memory Fetch')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','CACHE_SIZE',0,0,'Cache miss, could lead to HDD fetch')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','GRAPHICS_CARD_MEM_SIZE',1,1,'Supports High GPU Performance')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','GRAPHICS_CARD_MEM_SIZE',0,0,'Supports Low GPU Performance')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','OPERATING_SYSTEM',1,1,'Improves performance of 64 bit OS')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','OPERATING_SYSTEM',0,0,'Cannot run 64 bit OS.')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','NO_OF_USB_PORTS',1,1,'Improves performance')
INSERT INTO [dbo].[RULES] VALUES('RAM_SIZE','NO_OF_USB_PORTS',0,0,'Reduces performance')
INSERT INTO [dbo].[RULES] VALUES('DISPLAY_SIZE','BATTERY_LIFE',1,0,'Higher the display size, Lower the battery life.')
INSERT INTO [dbo].[RULES] VALUES('DISPLAY_SIZE','BATTERY_LIFE',0,1,'Lower the display size, higher the battery life.')
INSERT INTO [dbo].[RULES] VALUES('DISPLAY_SIZE','DISPLAY_SIZE',1,0,'Higher the display size, higher the weight.')
INSERT INTO [dbo].[RULES] VALUES('DISPLAY_SIZE','DISPLAY_SIZE',0,1,'Lower the display size, lower the weight.')
INSERT INTO [dbo].[RULES] VALUES('PROCESSOR_SPEED','PRICE',1,0,'Higher the processor speed, higher the price')
INSERT INTO [dbo].[RULES] VALUES('PROCESSOR_SPEED','PRICE',0,1,'Lower the processor speed, lower the price')
INSERT INTO [dbo].[RULES] VALUES('PROCESSOR_BRAND','PRICE',1,0,'Intel Processors are expensive compared to AMD.')
INSERT INTO [dbo].[RULES] VALUES('PROCESSOR_BRAND','PRICE',0,1,'AMD Processors are inexpensive.')
INSERT INTO [dbo].[RULES] VALUES('HDD_SIZE','PRICE',1,0,'Higher the HDD Size, higher the price.')
INSERT INTO [dbo].[RULES] VALUES('HDD_SIZE','PRICE',0,1,'Lower the HDD Size, lower the price.')
INSERT INTO [dbo].[RULES] VALUES('PRICE','PROCESSOR_SPEED',1,1,'Higher the price, higher the processor Speed.')
INSERT INTO [dbo].[RULES] VALUES('PRICE','PROCESSOR_SPEED',0,0,'Lower the price, lower the processor Speed.')
INSERT INTO [dbo].[RULES] VALUES('PRICE','HDD_SIZE',1,1,'Higher the HDD Size, higher the price.')
INSERT INTO [dbo].[RULES] VALUES('PRICE','HDD_SIZE',0,0,'Lower the HDD Size, lower the price.')
INSERT INTO [dbo].[RULES] VALUES('PRICE','PROCESSOR_BRAND',1,1,'Higher the price, better the brand i.e. intel')
INSERT INTO [dbo].[RULES] VALUES('PRICE','PROCESSOR_BRAND',0,0,'Lower the price, lower the brand value i.e. AMD')
INSERT INTO [dbo].[RULES] VALUES('PRICE','CACHE_SIZE',1,1,'Higher the price, higher the cache')
INSERT INTO [dbo].[RULES] VALUES('PRICE','CACHE_SIZE',0,0,'Lower the price, lower the cache')
INSERT INTO [dbo].[RULES] VALUES('PRICE','GRAPHICS_CARD_MEM_SIZE',1,1,'Higher the price, the graphics card memory is high and better')
INSERT INTO [dbo].[RULES] VALUES('PRICE','GRAPHICS_CARD_MEM_SIZE',0,0,'Lower the price, the less is the graphic card memory')
INSERT INTO [dbo].[RULES] VALUES('PRICE','OPERATING_SYSTEM',1,1,'Higher the price, better the OS e.g. Windows Premium')
INSERT INTO [dbo].[RULES] VALUES('PRICE','OPERATING_SYSTEM',0,0,'Lower the price, lower the OS Brand')
INSERT INTO [dbo].[RULES] VALUES('PRICE','WARRANTY',1,1,'Higher the price, the more is the warranty')
INSERT INTO [dbo].[RULES] VALUES('PRICE','WARRANTY',0,0,'Lower the price, the less is the warranty.')
INSERT INTO [dbo].[RULES] VALUES('PRICE','RAM_SIZE',1,1,'Higher the price, Higher RAM.')
INSERT INTO [dbo].[RULES] VALUES('PRICE','RAM_SIZE',0,0,'Low price, low RAM size.')
INSERT INTO [dbo].[RULES] VALUES('PRICE','DISPLAY_SIZE',1,1,'Higher price entails higher display size.')
INSERT INTO [dbo].[RULES] VALUES('PRICE','DISPLAY_SIZE',0,0,'Low price entails low display size.')
INSERT INTO [dbo].[RULES] VALUES('CACHE_SIZE','RAM_SIZE',1,1,'Higher the price, Higher RAM.')
INSERT INTO [dbo].[RULES] VALUES('CACHE_SIZE','RAM_SIZE',0,0,'Low price, low RAM size.')
INSERT INTO [dbo].[RULES] VALUES('CACHE_SIZE','DISPLAY_SIZE',1,1,'Higher price entails higher display size.')
INSERT INTO [dbo].[RULES] VALUES('CACHE_SIZE','DISPLAY_SIZE',0,0,'Low price entails low display size.')
INSERT INTO [dbo].[RULES] VALUES('CACHE_SIZE','PRICE',1,0,'Higher the cache size, higher the price')
INSERT INTO [dbo].[RULES] VALUES('CACHE_SIZE','PRICE',0,1,'Lower the cache size, lower the price.')
INSERT INTO [dbo].[RULES] VALUES('GRAPHICS_CARD_MEM_SIZE','RAM_SIZE',1,1,'Higher the graphics card, Higher RAM.')
INSERT INTO [dbo].[RULES] VALUES('GRAPHICS_CARD_MEM_SIZE','RAM_SIZE',0,0,'Low graphics card, low RAM size.')
INSERT INTO [dbo].[RULES] VALUES('GRAPHICS_CARD_MEM_SIZE','PRICE',1,0,'Higher the graphics card size and speed, higher the price')
INSERT INTO [dbo].[RULES] VALUES('GRAPHICS_CARD_MEM_SIZE','PRICE',0,1,'Lower the graphics card size and speed, lower the price.')
INSERT INTO [dbo].[RULES] VALUES('OPERATING_SYSTEM','RAM_SIZE',1,0,'Higher the OS version, More is the RAM requirement.')
INSERT INTO [dbo].[RULES] VALUES('OPERATING_SYSTEM','RAM_SIZE',0,1,'Lower the OS version , less the RAM size requirement.')
INSERT INTO [dbo].[RULES] VALUES('OPERATING_SYSTEM','WARRANTY',1,1,'Higher the price, the more is the warranty')
INSERT INTO [dbo].[RULES] VALUES('OPERATING_SYSTEM','WARRANTY',0,0,'Lower the price, the less is the warranty.')
INSERT INTO [dbo].[RULES] VALUES('OPERATING_SYSTEM','PRICE',1,0,'Better the OS, higher the price. E.g. windows is expensive than LINUX.')
INSERT INTO [dbo].[RULES] VALUES('OPERATING_SYSTEM','PRICE',0,1,'The OS is not good, the price is low. E.g Linux is inexpensive compared to windows')
INSERT INTO [dbo].[RULES] VALUES('NO_OF_CORES','PROCESSOR_SPEED',1,1,'More the no. of cores, more the overall speed.')
INSERT INTO [dbo].[RULES] VALUES('NO_OF_CORES','PROCESSOR_SPEED',0,0,'Less the no. of cores, less is the overall speed.')
INSERT INTO [dbo].[RULES] VALUES('NO_OF_CORES','PRICE',1,0,'More the number of cores, higher the price. E.g. windows is expensive than LINUX.')
INSERT INTO [dbo].[RULES] VALUES('NO_OF_CORES','PRICE',0,1,'Less the no. of cores, less is the price.')
INSERT INTO [dbo].[RULES] VALUES('NO_OF_CORES','WARRANTY',1,1,'Higher the price, the more is the warranty')
INSERT INTO [dbo].[RULES] VALUES('NO_OF_CORES','WARRANTY',0,0,'Lower the price, the less is the warranty.')

GO

CREATE TABLE [dbo].[PRODUCT_RANGE]
(
	ATTRIBUTE varchar(100),
	IMPORTANCE varchar(50),
	LOW_RANGE varchar(50),
)

GO
ALTER TABLE [dbo].[PRODUCT_RANGE] ALTER COLUMN ATTRIBUTE VARCHAR(100) NOT NULL;
GO
ALTER TABLE [dbo].[PRODUCT_RANGE] ALTER COLUMN IMPORTANCE VARCHAR(50) NOT NULL;
GO

ALTER TABLE [dbo].[PRODUCT_RANGE] ADD CONSTRAINT [PK_PRIMARYKEY] PRIMARY KEY CLUSTERED ([ATTRIBUTE] , [IMPORTANCE] )

GO

INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('BATTERY_LIFE','High',6)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('BATTERY_LIFE','Medium',5)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('BATTERY_LIFE','Low',3)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('CACHE_SIZE','High',5)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('CACHE_SIZE','Medium',3)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('CACHE_SIZE','Low',2)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('DISPLAY_SIZE','High',15)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('DISPLAY_SIZE','Medium',14)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('DISPLAY_SIZE','Low',12)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('GRAPHICS_CARD_MEM_SIZE','High',2.5)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('GRAPHICS_CARD_MEM_SIZE','Medium',1.5)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('GRAPHICS_CARD_MEM_SIZE','Low',0.3)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('HDD_SIZE','High',500)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('HDD_SIZE','Medium',400)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('HDD_SIZE','Low',300)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('NO_OF_CORES','High',6)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('NO_OF_CORES','Medium',4)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('NO_OF_CORES','Low',2)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('NO_OF_USB_PORTS','High',4)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('NO_OF_USB_PORTS','Medium',3)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('NO_OF_USB_PORTS','Low',2)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('OPERATING_SYSTEM_RANK','High',9)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('OPERATING_SYSTEM_RANK','Medium',7)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('OPERATING_SYSTEM_RANK','Low',5)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('PRICE','High',400)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('PRICE','Medium',600)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('PRICE','Low',800)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('PROCESSOR_SPEED','High',2.5)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('PROCESSOR_SPEED','Medium',2.2)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('PROCESSOR_SPEED','Low',1.5)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('RAM_SIZE','High',6)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('RAM_SIZE','Medium',4)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('RAM_SIZE','Low',2)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('WARRANTY','High',2)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('WARRANTY','Medium',1)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('WARRANTY','Low',0)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('WEIGHT_OF_PRODUCT','High',2)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('WEIGHT_OF_PRODUCT','Medium',7)
INSERT INTO [dbo].[PRODUCT_RANGE] VALUES('WEIGHT_OF_PRODUCT','Low',12)
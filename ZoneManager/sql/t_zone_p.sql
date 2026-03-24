/*
 Navicat MySQL Data Transfer

 Source Server         : 192.168.1.115
 Source Server Type    : MySQL
 Source Server Version : 80032
 Source Host           : 192.168.1.115:3306
 Source Schema         : 

 Target Server Type    : MySQL
 Target Server Version : 80032
 File Encoding         : 65001

 Date: 25/09/2024 11:32:48
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for t_zone_p
-- ----------------------------
DROP TABLE IF EXISTS `t_zone_p`;
CREATE TABLE `t_zone_p`  (
  `QYNM` varchar(50) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NOT NULL,
  `XH` decimal(4, 0) NOT NULL,
  `JD` decimal(10, 7) NOT NULL,
  `WD` decimal(10, 7) NOT NULL
) ENGINE = InnoDB CHARACTER SET = gb2312 COLLATE = gb2312_chinese_ci COMMENT = '区域_位置点' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of t_zone_p
-- ----------------------------
INSERT INTO `t_zone_p` VALUES ('6543', 0, 118.6220000, 29.7939000);
INSERT INTO `t_zone_p` VALUES ('6543', 1, 0.0000000, 360.0000000);
INSERT INTO `t_zone_p` VALUES ('6543', 2, 30.0000000, 0.0000000);

SET FOREIGN_KEY_CHECKS = 1;

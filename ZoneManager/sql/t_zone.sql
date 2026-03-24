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

 Date: 25/09/2024 11:32:36
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for t_zone
-- ----------------------------
DROP TABLE IF EXISTS `t_zone`;
CREATE TABLE `t_zone`  (
  `QYNM` varchar(50) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NOT NULL,
  `XIANXING` decimal(2, 0) NULL DEFAULT NULL,
  `QYLX` decimal(2, 0) NULL DEFAULT NULL,
  `QYXZ` decimal(2, 0) NULL DEFAULT NULL,
  `QYSG` decimal(10, 2) NULL DEFAULT NULL,
  `QYXG` decimal(10, 2) NULL DEFAULT NULL,
  `QYDS` decimal(3, 0) NULL DEFAULT NULL,
  `QYCOLOR` decimal(10, 0) NULL DEFAULT NULL,
  `TIANCHONG` decimal(3, 0) NULL DEFAULT NULL
) ENGINE = InnoDB CHARACTER SET = gb2312 COLLATE = gb2312_chinese_ci COMMENT = '区域' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of t_zone
-- ----------------------------
INSERT INTO `t_zone` VALUES ('6543', 1, 0, 0, 8000.00, 6000.00, 1, 16777130, 1);

SET FOREIGN_KEY_CHECKS = 1;

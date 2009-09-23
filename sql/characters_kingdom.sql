--
-- Table structure for table `kingdom`
--

DROP TABLE IF EXISTS `kingdom`;
CREATE TABLE `kingdom` (
  `kid` int(10) unsigned NOT NULL,
  `team` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY  (`kid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Aktualni data kralovstvi';

--
-- Dumping data for table `kingdom`
--


/*!40000 ALTER TABLE `kingdom` DISABLE KEYS */;
LOCK TABLES `kingdom` WRITE;
INSERT INTO `kingdom` VALUES (1,1);
UNLOCK TABLES;
/*!40000 ALTER TABLE `kingdom` ENABLE KEYS */;

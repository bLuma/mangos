--
-- Table structure for table `kingdom_creature`
--

DROP TABLE IF EXISTS `kingdom_creature`;
CREATE TABLE `kingdom_creature` (
  `kid` int(10) unsigned NOT NULL,
  `team` tinyint(3) unsigned NOT NULL,
  `guid` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='Definice priser z kralovstvi';

--
-- Table structure for table `kingdom_gameobject`
--

DROP TABLE IF EXISTS `kingdom_gameobject`;
CREATE TABLE `kingdom_gameobject` (
  `kid` int(10) unsigned NOT NULL default '0',
  `team` tinyint(3) unsigned NOT NULL default '0',
  `guid` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `kingdom_template`
--

DROP TABLE IF EXISTS `kingdom_template`;
CREATE TABLE `kingdom_template` (
  `kid` int(10) unsigned NOT NULL,
  `team` tinyint(3) unsigned NOT NULL,
  `name` varchar(50) NOT NULL default '',
  PRIMARY KEY  (`kid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Definice kralovstvi';

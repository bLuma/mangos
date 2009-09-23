--
-- Struktura tabulky `kingdom_creature`
--

DROP TABLE IF EXISTS `kingdom_creature`;
CREATE TABLE IF NOT EXISTS `kingdom_creature` (
  `kid` int(10) unsigned NOT NULL,
  `team` tinyint(3) unsigned NOT NULL,
  `guid` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Definice priser z kralovstvi';

-- --------------------------------------------------------

--
-- Struktura tabulky `kingdom_gameobject`
--

DROP TABLE IF EXISTS `kingdom_gameobject`;
CREATE TABLE IF NOT EXISTS `kingdom_gameobject` (
  `kid` int(10) unsigned NOT NULL,
  `team` tinyint(3) unsigned NOT NULL,
  `guid` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Definice gameobjektu z kralovstvi';

-- --------------------------------------------------------

--
-- Struktura tabulky `kingdom_template`
--

DROP TABLE IF EXISTS `kingdom_template`;
CREATE TABLE IF NOT EXISTS `kingdom_template` (
  `kid` int(10) unsigned NOT NULL,
  `team` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY  (`kid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Definice kralovstvi';

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';


-- -----------------------------------------------------
-- Table `Total`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `Total` (
  `id` INT NOT NULL ,
  `total` INT NULL DEFAULT 0 ,
  `unsolved` INT NULL DEFAULT 0 ,
  PRIMARY KEY (`id`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Server`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `Server` (
  `id` INT NOT NULL ,
  `name` VARCHAR(200) NULL ,
  `address` VARCHAR(200) NULL ,
  `occurrence` INT NULL DEFAULT 0 ,
  PRIMARY KEY (`id`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `MetaAgent`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `MetaAgent` (
  `id` INT NOT NULL ,
  `name` VARCHAR(200) NULL ,
  `type` VARCHAR(45) NULL ,
  `id_total` INT NULL ,
  `classification` VARCHAR(200) NULL ,
  `id_server` INT NULL ,
  `valid` TINYINT(1)  NULL DEFAULT 1 ,
  `local_total` INT NULL DEFAULT 0 ,
  `tolerate` INT NULL DEFAULT 0 ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_MetaAgent_2` (`id_total` ASC) ,
  INDEX `fk_MetaAgent_1` (`id_server` ASC) ,
  CONSTRAINT `fk_MetaAgent_2`
    FOREIGN KEY (`id_total` )
    REFERENCES `Total` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_MetaAgent_1`
    FOREIGN KEY (`id_server` )
    REFERENCES `Server` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Occurrence`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `Occurrence` (
  `id` INT NOT NULL ,
  `occurrence` INT NULL DEFAULT 0 ,
  PRIMARY KEY (`id`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `MetaAgentTree`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `MetaAgentTree` (
  `id` INT NOT NULL ,
  `id_parent` INT NULL ,
  `id_daughter` INT NULL ,
  `id_occurrence` INT NULL ,
  `local_occurrence` INT NULL DEFAULT 0 ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_MetaAgentTree_1` (`id_parent` ASC) ,
  INDEX `fk_MetaAgentTree_2` (`id_daughter` ASC) ,
  INDEX `fk_MetaAgentTree_3` (`id_occurrence` ASC) ,
  CONSTRAINT `fk_MetaAgentTree_1`
    FOREIGN KEY (`id_parent` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_MetaAgentTree_2`
    FOREIGN KEY (`id_daughter` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_MetaAgentTree_3`
    FOREIGN KEY (`id_occurrence` )
    REFERENCES `Occurrence` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Dependency`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `Dependency` (
  `id` INT NOT NULL ,
  `id_prior` INT NULL ,
  `id_after` INT NULL ,
  `occurrence` INT NULL DEFAULT 0 ,
  `validity` INT NULL DEFAULT 0 ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_Rule_1` (`id_prior` ASC) ,
  INDEX `fk_Rule_2` (`id_after` ASC) ,
  CONSTRAINT `fk_Rule_1`
    FOREIGN KEY (`id_prior` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Rule_2`
    FOREIGN KEY (`id_after` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `FileAgent`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `FileAgent` (
  `id` INT NOT NULL ,
  `id_metaAgent` INT NOT NULL ,
  `filename` VARCHAR(500) NULL ,
  `owner` VARCHAR(100) NULL DEFAULT '' ,
  `grp` VARCHAR(100) NULL DEFAULT '' ,
  `permissions` VARCHAR(4) NULL DEFAULT '' ,
  `attributes` VARCHAR(19) NULL DEFAULT '' ,
  `md5` VARCHAR(32) NULL DEFAULT '' ,
  `maxDepth` INT NULL DEFAULT 0 ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_FileAgent_1` (`id_metaAgent` ASC) ,
  CONSTRAINT `fk_FileAgent_1`
    FOREIGN KEY (`id_metaAgent` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `EnvironmentAgent`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `EnvironmentAgent` (
  `id` INT NOT NULL ,
  `id_metaAgent` INT NOT NULL ,
  `id_fstab` INT NULL ,
  `maxLoad` FLOAT NULL DEFAULT 2 ,
  `maxMemory` FLOAT NULL DEFAULT 100 ,
  `maxSwap` FLOAT NULL DEFAULT 99 ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_EnvironmentAgent_1` (`id_metaAgent` ASC) ,
  INDEX `fk_EnvironmentAgent_2` (`id_fstab` ASC) ,
  CONSTRAINT `fk_EnvironmentAgent_1`
    FOREIGN KEY (`id_metaAgent` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_EnvironmentAgent_2`
    FOREIGN KEY (`id_fstab` )
    REFERENCES `FileAgent` (`id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ProcessAgent`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `ProcessAgent` (
  `id` INT NOT NULL ,
  `id_metaAgent` INT NOT NULL ,
  `procName` VARCHAR(200) NULL ,
  `user` VARCHAR(100) NULL ,
  `id_procEnvFile` INT NULL ,
  `id_parent` INT NULL ,
  `multiplicity` INT NULL DEFAULT -1 ,
  `command` VARCHAR(300) NULL DEFAULT '' ,
  `service` VARCHAR(100) NULL DEFAULT '' ,
  `maxCpu` FLOAT NULL DEFAULT 100 ,
  `maxMemory` FLOAT NULL DEFAULT 100 ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_ProcessAgent_1` (`id_metaAgent` ASC) ,
  INDEX `fk_ProcessAgent_2` (`id_procEnvFile` ASC) ,
  INDEX `fk_ProcessAgent_3` (`id_parent` ASC) ,
  CONSTRAINT `fk_ProcessAgent_1`
    FOREIGN KEY (`id_metaAgent` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_ProcessAgent_2`
    FOREIGN KEY (`id_procEnvFile` )
    REFERENCES `FileAgent` (`id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_ProcessAgent_3`
    FOREIGN KEY (`id_parent` )
    REFERENCES `ProcessAgent` (`id` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `FileContentRules`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `FileContentRules` (
  `id` INT NOT NULL AUTO_INCREMENT ,
  `id_file` INT NOT NULL ,
  `rule` VARCHAR(10000) NULL ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_FileContentRules_1` (`id_file` ASC) ,
  CONSTRAINT `fk_FileContentRules_1`
    FOREIGN KEY (`id_file` )
    REFERENCES `FileAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Limits`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `Limits` (
  `id` INT NOT NULL ,
  `name` VARCHAR(45) NULL ,
  `defValue` BIGINT NOT NULL ,
  PRIMARY KEY (`id`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ProcessLimits`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `ProcessLimits` (
  `id` INT NOT NULL ,
  `id_proc` INT NULL ,
  `id_limit` INT NULL ,
  `value` FLOAT NOT NULL ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_ProcessLimits_1` (`id_proc` ASC) ,
  INDEX `fk_ProcessLimits_2` (`id_limit` ASC) ,
  CONSTRAINT `fk_ProcessLimits_1`
    FOREIGN KEY (`id_proc` )
    REFERENCES `ProcessAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_ProcessLimits_2`
    FOREIGN KEY (`id_limit` )
    REFERENCES `Limits` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `RecoveryRule`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `RecoveryRule` (
  `id` INT NOT NULL ,
  `id_ma1` INT NULL ,
  `action1` VARCHAR(45) NULL ,
  `id_ma2` INT NULL ,
  `action2` VARCHAR(45) NULL ,
  `direction` VARCHAR(45) NULL ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_RecoveryRule_1` (`id_ma1` ASC) ,
  INDEX `fk_RecoveryRule_2` (`id_ma2` ASC) ,
  CONSTRAINT `fk_RecoveryRule_1`
    FOREIGN KEY (`id_ma1` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_RecoveryRule_2`
    FOREIGN KEY (`id_ma2` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `RecoveryHistory`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `RecoveryHistory` (
  `id` INT NOT NULL AUTO_INCREMENT ,
  `date` VARCHAR(45) NULL ,
  `time` VARCHAR(45) NULL ,
  `responsible` VARCHAR(200) NULL ,
  `userEntry` VARCHAR(1000) NULL DEFAULT 'No comment' ,
  PRIMARY KEY (`id`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `RecoveryHistory_Problem`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `RecoveryHistory_Problem` (
  `id` INT NOT NULL AUTO_INCREMENT ,
  `id_recoveryHistory` INT NULL ,
  `metaAgent` VARCHAR(200) NULL ,
  `before_after` VARCHAR(45) NULL ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_RecoveryHistory_MetaAgent_1` (`id_recoveryHistory` ASC) ,
  CONSTRAINT `fk_RecoveryHistory_MetaAgent_1`
    FOREIGN KEY (`id_recoveryHistory` )
    REFERENCES `RecoveryHistory` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `RecoveryHistory_Action`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `RecoveryHistory_Action` (
  `id` INT NOT NULL AUTO_INCREMENT ,
  `id_recoveryHistory` INT NULL ,
  `metaAgent` VARCHAR(200) NULL ,
  `action` VARCHAR(45) NULL ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_RecoveryHistory_Action_1` (`id_recoveryHistory` ASC) ,
  CONSTRAINT `fk_RecoveryHistory_Action_1`
    FOREIGN KEY (`id_recoveryHistory` )
    REFERENCES `RecoveryHistory` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `FilenameFilter`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `FilenameFilter` (
  `id` INT NOT NULL AUTO_INCREMENT ,
  `id_folder` INT NULL ,
  `filter` VARCHAR(10000) NOT NULL ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_filenameFilter_1` (`id_folder` ASC) ,
  CONSTRAINT `fk_filenameFilter_1`
    FOREIGN KEY (`id_folder` )
    REFERENCES `FileAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `IgnoredFS`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `IgnoredFS` (
  `id` INT NOT NULL AUTO_INCREMENT ,
  `id_envAgent` INT NULL ,
  `fsName` VARCHAR(200) NULL ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_IgnoredFS_1` (`id_envAgent` ASC) ,
  CONSTRAINT `fk_IgnoredFS_1`
    FOREIGN KEY (`id_envAgent` )
    REFERENCES `EnvironmentAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Veto`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `Veto` (
  `id` INT NOT NULL AUTO_INCREMENT ,
  `idCoord` INT NULL ,
  `name` VARCHAR(200) NULL ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_Veto_1` (`idCoord` ASC) ,
  CONSTRAINT `fk_Veto_1`
    FOREIGN KEY (`idCoord` )
    REFERENCES `MetaAgent` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

-- -----------------------------------------------------
-- Data for table `Limits`
-- -----------------------------------------------------
START TRANSACTION;
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (0, 'MAX_CPU_TIME', 4294967295);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (1, 'MAX_FILE_SIZE', 4294967295);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (2, 'MAX_DATA_SIZE', 4294967295);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (3, 'MAX_STACK_SIZE', 8388608);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (4, 'MAX_CORE_FILE_SIZE', 0);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (5, 'MAX_RESIDENT_SET', 4294967295);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (6, 'MAX_PROCESSES', 1024);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (7, 'MAX_OPEN_FILES', 1024);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (8, 'MAX_LOCKED_MEMORY', 32768);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (9, 'MAX_ADDRESS_SPACE', 4294967295);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (10, 'MAX_FILE_LOCKS', 4294967295);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (11, 'MAX_PENDING_SIGNALS', 14818);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (12, 'MAX_MSGQUEUE_SIZE', 819200);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (13, 'MAX_NICE_PRIORITY', 0);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (14, 'MAX_REALTIME_PRIORITY', 0);
INSERT INTO Limits (`id`, `name`, `defValue`) VALUES (15, 'MAX_REALTIME_TIMEOUT', 4294967295);

COMMIT;

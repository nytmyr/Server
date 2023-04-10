UPDATE rule_values
SET notes = "0 = allow overwrites/rule disabled. If set to 1 = Block all buffs that would overwrite Resurrection Effects. If set to 2 = Will not overwrite Resurrection Effects, instead moves new buff to an empty slot if available. Default is 2."
WHERE rule_name LIKE 'Spells:ResurrectionEffectsBlock';
UPDATE rule_values
SET rule_value = 
	(CASE
		WHEN rule_value = "true"
			THEN 2
		ELSE 0
	END)
WHERE rule_name LIKE 'Spells:ResurrectionEffectsBlock';
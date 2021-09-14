import React, { useEffect, useState } from 'react';
import Box from '@material-ui/core/Box';
import { readPpm } from '../../tools/firebase';

export const Text = ({ text1, text2 }) => {
	const [ppm, setPpm] = useState();
	useEffect(() => {
		const tmp = readPpm();
		setPpm(tmp);
	}, [ppm]);
	return (
		<div>
			<Box component="span" display="block" p={1} m={1} bgcolor="background.paper" fontSize="25px">
				{`${text1} ${text2}`}
			</Box>
		</div>
	);
};

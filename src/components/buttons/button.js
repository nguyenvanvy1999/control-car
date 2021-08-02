import React from 'react';
import Button from '@material-ui/core/Button';
import { writeControl } from '../../tools/firebase';

export const CustomButton = ({ onClick, name }) => {
	return (
		<div className="control-button">
			<Button variant="contained" color="secondary" onClick={async () => await writeControl(onClick)}>
				{name}
			</Button>
		</div>
	);
};

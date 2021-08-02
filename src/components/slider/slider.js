import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import Slider from '@material-ui/core/Slider';
import { writeSpeed } from '../../tools/firebase';

const useStyles = makeStyles({
	root: {
		margin: 200,
		width: 300,
		display: 'inline-block',
	},
});

async function valueText(value) {
	const control = (parseInt(value) / 10).toString();
	return await writeSpeed(control);
}

export const CustomSlider = () => {
	const classes = useStyles();

	return (
		<div className={classes.root}>
			<Typography id="discrete-slider" gutterBottom>
				Speed
			</Typography>
			<Slider
				defaultValue={50}
				getAriaValueText={valueText}
				aria-labelledby="discrete-slider"
				valueLabelDisplay="auto"
				step={10}
				marks
				min={0}
				max={90}
			/>
		</div>
	);
};

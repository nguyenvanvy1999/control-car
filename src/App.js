import './App.css';
import { CustomButton } from './components/buttons/button';
import { CustomSlider } from './components/slider/slider';
function App() {
	return (
		<div>
			<CustomButton name={'Tiến'} onClick={'F'} />
			<CustomButton name={'Lùi'} onClick={'B'} />
			<CustomButton name={'Trái'} onClick={'L'} />
			<CustomButton name={'Phải'} onClick={'R'} />
			<CustomButton name={'Tiến Trái'} onClick={'G'} />
			<CustomButton name={'Tiến Phải'} onClick={'I'} />
			<CustomButton name={'Lùi trái'} onClick={'H'} />
			<CustomButton name={'Lùi Phải'} onClick={'J'} />
			<CustomButton name={'Dừng'} onClick={'S'} />
			<CustomSlider />
		</div>
	);
}

export default App;

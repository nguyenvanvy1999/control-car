import './App.css';
import { CustomButton } from './components/buttons/button';
import { CustomSlider } from './components/slider/slider';
import { Text } from './components/text/text';

function App() {
	return (
		<div className="main">
			<div className="button">
				<div>
					<div className="app">
						<CustomButton name={'Tiến Trái'} onClick={'G'} />
						<CustomButton name={'Tiến Phải'} onClick={'I'} />
						<CustomButton name={'Tiến'} onClick={'F'} />
					</div>
					<div className="app">
						<CustomButton name={'Trái'} onClick={'L'} />
						<CustomButton name={'Dừng'} onClick={'S'} />
						<CustomButton name={'Phải'} onClick={'R'} />
					</div>
					<div className="app">
						<CustomButton name={'Lùi trái'} onClick={'H'} />
						<CustomButton name={'Lùi'} onClick={'B'} />
						<CustomButton name={'Lùi Phải'} onClick={'J'} />
					</div>
				</div>
			</div>
			<div className="slider">
				<CustomSlider />
			</div>
			<div className="text">
				<Text />
			</div>
		</div>
	);
}

export default App;

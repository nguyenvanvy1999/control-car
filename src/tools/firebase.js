import firebase from 'firebase';

const config = {
	apiKey: 'AIzaSyCDwdlZzKG1GLOUvwTCzdsDbOoWQ88jTIg',
	authDomain: 'projectdemo-a4fe4.firebaseapp.com',
	databaseURL: 'https://projectdemo-a4fe4-default-rtdb.asia-southeast1.firebasedatabase.app',
	projectId: 'projectdemo-a4fe4',
	storageBucket: 'projectdemo-a4fe4.appspot.com',
	messagingSenderId: '302708834870',
	appId: '1:302708834870:web:b538dd9498e50551c18e97',
	measurementId: 'G-KPV7CTPXQ4',
};

if (!firebase.apps.length) {
	try {
		firebase.initializeApp(config);
		firebase.analytics();
	} catch (error) {
		throw error;
	}
}

const realtimeDB = firebase.database();

export const writeControl = async (control) => {
	await realtimeDB.ref('control/').set(control);
};

export const writeSpeed = async (speed) => {
	await realtimeDB.ref('speed/').set(speed);
};

// Future functionality: Adding interactivity or animations

// Example: Form submission confirmation (client-side validation)
document.querySelector('form').addEventListener('submit', function (event) {
    const username = document.getElementById('username').value;
    const train = document.getElementById('train').value;
    const date = document.getElementById('date').value;
  
    if (username && train && date) {
      alert(`Reservation confirmed for ${username} on train ${train} on ${date}!`);
    } else {
      alert('Please fill in all fields before submitting.');
      event.preventDefault(); // Prevent form submission if validation fails
    }
  });
  